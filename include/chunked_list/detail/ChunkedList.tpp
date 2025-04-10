#pragma once

#include <iostream>

#include "../ChunkedList.hpp"
#include "utility.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::BoundaryError::BoundaryError(const char *message) : message{message} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::BoundaryError::BoundaryError(const std::string &message) : message{message} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::BoundaryError::BoundaryError(std::string &&message) :
      message{std::move(message)} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  const char *ChunkedList<T, ChunkSize, Allocator>::BoundaryError::what() const noexcept {
    return message.data();
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<bool PrevAssigned>
  void ChunkedList<T, ChunkSize, Allocator>::pushChunk(Chunk *chunk) {
    back->nextChunk = chunk;

    if (!PrevAssigned) {
      chunk->prevChunk = back;
    }

    back = chunk;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::ChunkedList() :
      front{ChunkAllocatorTraits::allocate(chunkAllocator, 1)}, back{front} {
    ChunkAllocatorTraits::construct(chunkAllocator, front);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::ChunkedList(std::initializer_list<T> initializerList) {
    if (initializerList.size() == 0) {
      back = front = ChunkAllocatorTraits::allocate(chunkAllocator, 1);
      ChunkAllocatorTraits::construct(chunkAllocator, front);
      return;
    }

    if (ChunkSize >= initializerList.size()) {
      back = front = ChunkAllocatorTraits::allocate(chunkAllocator, 1);
      ChunkAllocatorTraits::construct(chunkAllocator, front, initializerList.begin(), initializerList.size());
      return;
    }

    chunkCount = (initializerList.size() + ChunkSize - 1) / ChunkSize;

    front = back = ChunkAllocatorTraits::allocate(chunkAllocator, 1);
    ChunkAllocatorTraits::construct(chunkAllocator, front, initializerList.begin(), ChunkSize);

    for (size_t offset = 1; offset < chunkCount - 1; ++offset) {
      Chunk *chunk = ChunkAllocatorTraits::allocate(chunkAllocator, 1);
      ChunkAllocatorTraits::construct(chunkAllocator, chunk, initializerList.begin() + offset * ChunkSize, ChunkSize,
                                      back);
      pushChunk<true>(chunk);
    }

    const int remainingItems = initializerList.size() % ChunkSize;

    Chunk *ultimateChunk = ChunkAllocatorTraits::allocate(chunkAllocator, 1);

    ChunkAllocatorTraits::construct(chunkAllocator, ultimateChunk,
                                    initializerList.begin() + (chunkCount - 1) * ChunkSize,
                                    remainingItems == 0 ? ChunkSize : remainingItems, back);

    pushChunk<true>(ultimateChunk);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::~ChunkedList() {
    do {
      Chunk *prev = back->prevChunk;
      ChunkAllocatorTraits::destroy(chunkAllocator, back);
      ChunkAllocatorTraits::deallocate(chunkAllocator, back, 1);
      back = prev;
    } while (back);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  T &ChunkedList<T, ChunkSize, Allocator>::operator[](const size_t index) {
    Chunk *chunk = front;

    for (size_t chunkIndex = index / ChunkSize; chunkIndex > 0; --chunkIndex) {
      chunk = chunk->nextChunk;
    }

    return (*chunk)[index % ChunkSize];
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  const T &ChunkedList<T, ChunkSize, Allocator>::operator[](const size_t index) const {
    return const_cast<ChunkedList *>(this)->operator[](index);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  const T &ChunkedList<T, ChunkSize, Allocator>::at(const size_t index) const {
    if (index >= size()) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  T &ChunkedList<T, ChunkSize, Allocator>::at(const size_t index) {
    if (index >= size()) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Iterator ChunkedList<T, ChunkSize, Allocator>::begin() {
    return Iterator{front, 0};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::ConstIterator ChunkedList<T, ChunkSize, Allocator>::begin() const {
    return ConstIterator{front, 0};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Iterator ChunkedList<T, ChunkSize, Allocator>::end() {
    return Iterator{back, back->size()};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::ConstIterator ChunkedList<T, ChunkSize, Allocator>::end() const {
    return ConstIterator{back, back->size()};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::ChunkIterator ChunkedList<T, ChunkSize, Allocator>::beginChunk() {
    return ChunkIterator{front};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::ConstChunkIterator
  ChunkedList<T, ChunkSize, Allocator>::beginChunk() const {
    return ConstChunkIterator{front};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::ChunkIterator ChunkedList<T, ChunkSize, Allocator>::endChunk() {
    return ChunkIterator{nullptr};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::ConstChunkIterator
  ChunkedList<T, ChunkSize, Allocator>::endChunk() const {
    return ConstChunkIterator{nullptr};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Slice
  ChunkedList<T, ChunkSize, Allocator>::slice(const size_t startIndex, const size_t endIndex) {
    ChunkIterator chunkIt{front};

    const size_t startChunkIndex = startIndex / ChunkSize;

    for (size_t i = 0; i < startChunkIndex; ++i) {
      ++chunkIt;
    }

    Chunk &startChunk = *chunkIt;

    const size_t endChunkIndex = endIndex / ChunkSize;

    for (size_t i = startChunkIndex; i < endChunkIndex; ++i) {
      ++chunkIt;
    }

    return Slice{Iterator{startChunk, startIndex % ChunkSize}, Iterator{*chunkIt, endIndex % ChunkSize}};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::ConstSlice
  ChunkedList<T, ChunkSize, Allocator>::slice(const size_t startIndex, const size_t endIndex) const {
    return ConstSlice{const_cast<ChunkedList *>(this)->slice(startIndex, endIndex)};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::all_iterators_or_chunk_iterators<ChunkedList<T, ChunkSize, Allocator>, StartIteratorT,
                                                       EndIteratorT>
  typename ChunkedList<T, ChunkSize, Allocator>::Slice ChunkedList<T, ChunkSize, Allocator>::slice(StartIteratorT start,
                                                                                                   EndIteratorT end) {
    return Slice{start, end};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::all_iterators_or_chunk_iterators<ChunkedList<T, ChunkSize, Allocator>, StartIteratorT,
                                                       EndIteratorT>
  typename ChunkedList<T, ChunkSize, Allocator>::ConstSlice
  ChunkedList<T, ChunkSize, Allocator>::slice(StartIteratorT start, EndIteratorT end) const {
    return ConstSlice{start, end};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::push(T value) {
    if (back->size() == ChunkSize) {
      Chunk *chunk = ChunkAllocatorTraits::allocate(chunkAllocator, 1);
      ChunkAllocatorTraits::construct(chunkAllocator, chunk, std::forward<T>(value), back);
      pushChunk<true>(chunk);
      ++chunkCount;
    } else {
      back->push(std::forward<T>(value));
    }
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename... Args>
    requires utility::can_construct<T, Args...>
  void ChunkedList<T, ChunkSize, Allocator>::emplace(Args &&...args) {
    if (back->size() == ChunkSize) {
      Chunk *chunk = ChunkAllocatorTraits::allocate(chunkAllocator, 1);
      ChunkAllocatorTraits::construct(chunkAllocator, chunk, T{std::forward<Args>(args)}..., back);
      pushChunk<true>(chunk);
    } else {
      back->emplace(args...);
    }
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<bool DestroyValue>
  void ChunkedList<T, ChunkSize, Allocator>::pop() {
    if (back->empty()) {
      popChunk();
    } else {
      back->template pop<DestroyValue>();
    }
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Iterator
  ChunkedList<T, ChunkSize, Allocator>::erase(Iterator iterator) {
    Iterator nextIt = ++iterator;
    Iterator firstIt = iterator, secondIt = nextIt, endIt = end();

    while (secondIt != endIt) {
      *firstIt = std::move(*secondIt);
      firstIt = secondIt;
      ++secondIt;
    }

    return nextIt;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::ChunkIterator
  ChunkedList<T, ChunkSize, Allocator>::erase(ChunkIterator iterator) {
    Chunk *prev = iterator->prevChunk;
    Chunk *next = iterator->nextChunk;

    ChunkAllocatorTraits::destroy(chunkAllocator, iterator);

    if (prev) {
      if (next) {
        prev->nextChunk = next;
        next->prevChunk = prev;
      } else {
        back = prev;
      }
    } else if (next) {
      front = next;

    } else {
      ChunkAllocatorTraits::construct(chunkAllocator, iterator);
    }

    return ChunkIterator{next};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::popChunk() {
    if (Chunk *prev = back->prevChunk; prev) {
      ChunkAllocatorTraits::destroy(chunkAllocator, back);
      ChunkAllocatorTraits::deallocate(chunkAllocator, back, 1);
      back = prev;
      --chunkCount;
      back->pop();
    } else {
      back->clear();
    }
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<bool DestroyFront>
  void ChunkedList<T, ChunkSize, Allocator>::clear() {
    while (back != front) {
      Chunk *prev = back->prevChunk;
      ChunkAllocatorTraits::destroy(chunkAllocator, back);
      ChunkAllocatorTraits::deallocate(chunkAllocator, back, 1);
      back = prev;
    }

    if constexpr (DestroyFront) {
      ChunkAllocatorTraits::destroy(chunkAllocator, front);
      ChunkAllocatorTraits::construct(chunkAllocator, front);
    } else {
      front->clear();
    }

    chunkCount = 1;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename Compare, utility::SortType Sort>
  void ChunkedList<T, ChunkSize, Allocator>::sort() {
    using namespace utility;
    using namespace sort_functions;

    switch (Sort) {
      case BubbleSort:
        return bubble_sort<Compare>(*this);
      case SelectionSort:
        return selection_sort<Compare>(*this);
      case InsertionSort:
        return insertion_sort<Compare>(*this);
      case QuickSort:
        return quick_sort<Compare, T, ChunkSize, Allocator>(begin(), end());
      case HeapSort:
        return heap_sort<Compare>(*this);
    }
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  size_t ChunkedList<T, ChunkSize, Allocator>::size() const {
    return (chunkCount - 1) * ChunkSize + back->size();
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::empty() const {
    return back->empty();
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::operator==(const ChunkedList &other) const {
    if (size() != other.size())
      return false;

    for (ConstIterator thisIterator = begin(), otherIterator = other.begin(); thisIterator != end();
         ++thisIterator, ++otherIterator)
      if (*thisIterator != *otherIterator)
        return false;

    return true;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::operator!=(const ChunkedList &other) const {
    if (size() != other.size())
      return true;

    for (ConstIterator thisIterator = begin(), otherIterator = other.begin(); thisIterator != end();
         ++thisIterator, ++otherIterator)
      if (*thisIterator != *otherIterator)
        return true;

    return false;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
    requires utility::can_insert<std::ostream, T>
  std::ostream &operator<<(std::ostream &os, ChunkedList<T, ChunkSize, Allocator> &chunkedList) {
    os << '[';

    auto iterator = chunkedList.begin(), end = chunkedList.end();

    os << *iterator;
    ++iterator;

    while (iterator != end) {
      os << ", " << *iterator;
      ++iterator;
    }

    return os << ']';
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename OutputStream, typename DelimiterType>
    requires utility::can_insert<OutputStream, T> && utility::can_insert<OutputStream, DelimiterType> &&
             utility::can_stringify<OutputStream>
  auto ChunkedList<T, ChunkSize, Allocator>::concat(const DelimiterType delimiter)
    -> utility::DeduceStreamStringType<OutputStream> {
    using StringType = utility::DeduceStreamStringType<OutputStream>;

    if (empty()) {
      return StringType{};
    }

    OutputStream stream;

    Iterator it = begin(), endIt = end();

    stream << *it;
    ++it;

    while (it != endIt) {
      stream << delimiter << *it;
      ++it;
    }

    return stream.str();
  }
} // namespace chunked_list

template<typename T, size_t ChunkSize, typename Allocator>
typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::Iterator
begin(chunked_list::ChunkedList<T, ChunkSize, Allocator> &chunkedList) noexcept {
  return chunkedList.begin();
}

template<typename T, size_t ChunkSize, typename Allocator>
typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::ConstIterator
begin(const chunked_list::ChunkedList<T, ChunkSize, Allocator> &chunkedList) noexcept {
  return chunkedList.begin();
}

template<typename T, size_t ChunkSize, typename Allocator>
typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::Iterator
end(chunked_list::ChunkedList<T, ChunkSize, Allocator> &chunkedList) noexcept {
  return chunkedList.end();
}

template<typename T, size_t ChunkSize, typename Allocator>
typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::ConstIterator
end(const chunked_list::ChunkedList<T, ChunkSize, Allocator> &chunkedList) noexcept {
  return chunkedList.end();
}

template<typename T, size_t ChunkSize, typename Allocator>
typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::Iterator
begin(typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::Chunk &chunk) noexcept {
  return chunk.begin();
}

template<typename T, size_t ChunkSize, typename Allocator>
typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::ConstIterator
begin(const typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::Chunk &chunk) noexcept {
  return chunk.begin();
}

template<typename T, size_t ChunkSize, typename Allocator>
typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::Iterator
end(typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::Chunk &chunk) noexcept {
  return chunk.end();
}

template<typename T, size_t ChunkSize, typename Allocator>
typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::ConstIterator
end(const typename chunked_list::ChunkedList<T, ChunkSize, Allocator>::Chunk &chunk) noexcept {
  return chunk.end();
}
