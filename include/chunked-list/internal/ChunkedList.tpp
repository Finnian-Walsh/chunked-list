#pragma once

#include <iostream>

#include "../ChunkedList.hpp"
#include "ChunkedListUtility.hpp"

namespace chunked_list {

  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::BoundaryError::BoundaryError(const char *message) : message{message} {}

  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::BoundaryError::BoundaryError(const std::string &message) : message{message} {}

  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::BoundaryError::BoundaryError(std::string &&message) : message{std::move(message)} {}

  template<typename T, size_t ChunkSize>
  const char *ChunkedList<T, ChunkSize>::BoundaryError::what() const noexcept {
    return message.data();
  }

  template<typename T, size_t ChunkSize>
  template<bool PrevAssigned>
  void ChunkedList<T, ChunkSize>::pushChunk(Chunk *chunk) {
    back->nextChunk = chunk;

    if (!PrevAssigned) {
      chunk->prevChunk = back;
    }

    back = chunk;
  }

  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::ChunkedList() : front{new Chunk{}}, back{front} {}

  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::ChunkedList(std::initializer_list<T> initializerList) {
    if (initializerList.size() == 0) {
      front = back = new Chunk{};
      return;
    }

    if (ChunkSize >= initializerList.size()) {
      front = back = new Chunk{initializerList.begin(), initializerList.size()};
      return;
    }

    front = back = new Chunk{initializerList.begin(), ChunkSize};

    chunkCount = (initializerList.size() + ChunkSize - 1) / ChunkSize;

    for (size_t offset = 1; offset < chunkCount - 1; ++offset) {
      pushChunk<true>(new Chunk{initializerList.begin() + (offset * ChunkSize), ChunkSize, back});
    }

    const int remainingItems = initializerList.size() % ChunkSize;
    pushChunk<true>(new Chunk{
      initializerList.begin() + (chunkCount - 1) * ChunkSize,
      remainingItems == 0 ? ChunkSize : remainingItems,
      back,
    });
  }

  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::~ChunkedList() {
    do {
      Chunk *newBack = back->prevChunk;
      delete back;
      back = newBack;
    } while (back);
  }

  template<typename T, size_t ChunkSize>
  T &ChunkedList<T, ChunkSize>::operator[](const size_t index) {
    Chunk *chunk = front;

    for (size_t chunkIndex = index / ChunkSize; chunkIndex > 0; --chunkIndex) {
      chunk = chunk->nextChunk;
    }

    return (*chunk)[index % ChunkSize];
  }

  template<typename T, size_t ChunkSize>
  const T &ChunkedList<T, ChunkSize>::operator[](const size_t index) const {
    return const_cast<ChunkedList *>(this)->operator[](index);
  }

  template<typename T, size_t ChunkSize>
  const T &ChunkedList<T, ChunkSize>::at(const size_t index) const {
    if (index >= size()) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize>
  T &ChunkedList<T, ChunkSize>::at(const size_t index) {
    if (index >= size()) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Iterator ChunkedList<T, ChunkSize>::begin() {
    return Iterator{front, 0};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::ConstIterator ChunkedList<T, ChunkSize>::begin() const {
    return ConstIterator{front, 0};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Iterator ChunkedList<T, ChunkSize>::end() {
    return Iterator{back, back->size()};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::ConstIterator ChunkedList<T, ChunkSize>::end() const {
    return ConstIterator{back, back->size()};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::ChunkIterator ChunkedList<T, ChunkSize>::beginChunk() {
    return ChunkIterator{front};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::ConstChunkIterator ChunkedList<T, ChunkSize>::beginChunk() const {
    return ConstChunkIterator{front};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::ChunkIterator ChunkedList<T, ChunkSize>::endChunk() {
    return ChunkIterator{nullptr};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::ConstChunkIterator ChunkedList<T, ChunkSize>::endChunk() const {
    return ConstChunkIterator{nullptr};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Slice ChunkedList<T, ChunkSize>::slice(const size_t startIndex,
                                                                             const size_t endIndex) {
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

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::ConstSlice ChunkedList<T, ChunkSize>::slice(const size_t startIndex,
                                                                                  const size_t endIndex) const {
    return ConstSlice{const_cast<ChunkedList *>(this)->slice(startIndex, endIndex)};
  }

  template<typename T, size_t ChunkSize>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::are_iterators_or_chunk_iterators<ChunkedList<T, ChunkSize>, StartIteratorT, EndIteratorT>
  typename ChunkedList<T, ChunkSize>::Slice ChunkedList<T, ChunkSize>::slice(StartIteratorT start, EndIteratorT end) {
    return Slice{start, end};
  }

  template<typename T, size_t ChunkSize>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::are_iterators_or_chunk_iterators<ChunkedList<T, ChunkSize>, StartIteratorT, EndIteratorT>
  typename ChunkedList<T, ChunkSize>::ConstSlice ChunkedList<T, ChunkSize>::slice(StartIteratorT start,
                                                                                  EndIteratorT end) const {
    return ConstSlice{start, end};
  }

  template<typename T, size_t ChunkSize>
  void ChunkedList<T, ChunkSize>::push(T value) {
    if (back->size() == ChunkSize) {
      pushChunk<true>(new Chunk{std::forward<T>(value), back});
      ++chunkCount;
    } else {
      back->push(std::forward<T>(value));
    }
  }

  template<typename T, size_t ChunkSize>
  template<typename... Args>
    requires utility::can_construct<T, Args...>
  void ChunkedList<T, ChunkSize>::emplace(Args &&...args) {
    if (back->size() == ChunkSize) {
      pushChunk(new Chunk{T(std::forward<Args>(args)...)});
    } else {
      back->emplace(args...);
    }
  }

  template<typename T, size_t ChunkSize>
  void ChunkedList<T, ChunkSize>::pop() {
    if (back->empty()) {
      popChunk();
    } else {
      back->pop();
    }
  }

  template<typename T, size_t ChunkSize>
  void ChunkedList<T, ChunkSize>::popChunk() {
    if (Chunk *newBack = back->prevChunk; newBack) {
      delete back;
      back = newBack;
      --chunkCount;
      back->pop();
    }
  }

  template<typename T, size_t ChunkSize>
  template<typename Compare, utility::SortType Sort>
  void ChunkedList<T, ChunkSize>::sort() {
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
        return quick_sort<Compare, T, ChunkSize>(begin(), end());
      case HeapSort:
        return heap_sort<Compare>(*this);
    }
  }

  template<typename T, size_t ChunkSize>
  size_t ChunkedList<T, ChunkSize>::size() const {
    return (chunkCount - 1) * ChunkSize + back->size();
  }

  template<typename T, size_t ChunkSize>
  bool ChunkedList<T, ChunkSize>::empty() const {
    return back->empty();
  }

  template<typename T, size_t ChunkSize>
  bool ChunkedList<T, ChunkSize>::operator==(const ChunkedList &other) const {
    if (size() != other.size())
      return false;

    for (ConstIterator thisIterator = begin(), otherIterator = other.begin(); thisIterator != end();
         ++thisIterator, ++otherIterator)
      if (*thisIterator != *otherIterator)
        return false;

    return true;
  }

  template<typename T, size_t ChunkSize>
  bool ChunkedList<T, ChunkSize>::operator!=(const ChunkedList &other) const {
    if (size() != other.size())
      return true;

    for (ConstIterator thisIterator = begin(), otherIterator = other.begin(); thisIterator != end();
         ++thisIterator, ++otherIterator)
      if (*thisIterator != *otherIterator)
        return true;

    return false;
  }

  template<typename T, size_t ChunkSize>
    requires utility::can_insert<std::ostream, T>
  std::ostream &operator<<(std::ostream &os, ChunkedList<T, ChunkSize> &chunkedList) {
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

  template<typename T, size_t ChunkSize>
  template<typename OutputStream, typename DelimiterType>
    requires utility::can_insert<OutputStream, T> && utility::can_insert<OutputStream, DelimiterType> &&
             utility::can_stringify<OutputStream>
  auto ChunkedList<T, ChunkSize>::concat(const DelimiterType delimiter)
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

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator
begin(chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept {
  return chunkedList.begin();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator
begin(const chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept {
  return chunkedList.begin();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator
end(chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept {
  return chunkedList.end();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator
end(const chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept {
  return chunkedList.end();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator
begin(typename chunked_list::ChunkedList<T, ChunkSize>::Chunk &chunk) noexcept {
  return chunk.begin();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator
begin(const typename chunked_list::ChunkedList<T, ChunkSize>::Chunk &chunk) noexcept {
  return chunk.begin();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator
end(typename chunked_list::ChunkedList<T, ChunkSize>::Chunk &chunk) noexcept {
  return chunk.end();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator
end(const typename chunked_list::ChunkedList<T, ChunkSize>::Chunk &chunk) noexcept {
  return chunk.end();
}
