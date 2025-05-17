#pragma once

#include <iostream>

#include "../ChunkedList.hpp"
#include "utility.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  constexpr typename ChunkedList<T, ChunkSize, Allocator>::Chunk *ChunkedList<T, ChunkSize, Allocator>::get_sentinel() {
    return reinterpret_cast<Chunk *>(&sentinel);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  constexpr const typename ChunkedList<T, ChunkSize, Allocator>::Chunk *
  ChunkedList<T, ChunkSize, Allocator>::get_sentinel() const {
    return reinterpret_cast<const Chunk *>(&sentinel);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::ChunkedList() : chunk_count{0} {
    new (&sentinel) Chunk{get_sentinel(), get_sentinel()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::ChunkedList(std::initializer_list<T> initializerList) {
    if (initializerList.size() == 0) {
      new (&sentinel) Chunk{get_sentinel(), get_sentinel()};
      return;
    }

    if (ChunkSize >= initializerList.size()) {
      chunk_count = 1;
      new (&sentinel) Chunk{initializerList.begin(), initializerList.size(), get_sentinel(), get_sentinel()};
      return;
    }

    chunk_count = (initializerList.size() + ChunkSize - 1) / ChunkSize;

    Chunk *frontPtr = ChunkAllocatorTraits::allocate(chunk_allocator, 1);
    Chunk *backPtr = ChunkAllocatorTraits::allocate(chunk_allocator, 1);

    new (frontPtr) Chunk{initializerList.begin(), ChunkSize, get_sentinel(), nullptr};
    new (&sentinel) Chunk{backPtr, frontPtr};

    Chunk *lastChunkPtr = frontPtr;

    for (size_t offset = 1; offset < chunk_count - 1; ++offset) {
      Chunk *chunkPtr = ChunkAllocatorTraits::allocate(chunk_allocator, 1);
      new (chunkPtr) Chunk{initializerList.begin() + offset * ChunkSize, ChunkSize, lastChunkPtr};
      lastChunkPtr->nextChunk = chunkPtr;
      lastChunkPtr = chunkPtr;
    }

    lastChunkPtr->nextChunk = backPtr;

    const int remainingItems = initializerList.size() % ChunkSize;

    new (backPtr) Chunk{initializerList.begin() + (chunk_count - 1) * ChunkSize,
                        remainingItems == 0 ? ChunkSize : remainingItems, lastChunkPtr, get_sentinel()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::~ChunkedList() {
    Chunk *currentPtr = get_sentinel()->prevChunk;

    while (currentPtr != get_sentinel()) {
      Chunk *prevPtr = currentPtr->prevChunk;
      std::destroy_at(currentPtr);
      ChunkAllocatorTraits::deallocate(chunk_allocator, currentPtr, 1);
      currentPtr = prevPtr;
    }

    std::destroy_at(get_sentinel());
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::BoundaryError::BoundaryError(const char *message) : message{message} {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::BoundaryError::BoundaryError(const std::string &message) : message{message} {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::BoundaryError::BoundaryError(std::string &&message) :
      message{std::move(message)} {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  const char *ChunkedList<T, ChunkSize, Allocator>::BoundaryError::what() const noexcept {
    return message.data();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  T &ChunkedList<T, ChunkSize, Allocator>::operator[](const size_t index) {
    Chunk *chunk = get_sentinel()->nextChunk;

    for (size_t chunkIndex = index / ChunkSize; chunkIndex > 0; --chunkIndex) {
      chunk = chunk->nextChunk;
    }

    return (*chunk)[index % ChunkSize];
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  const T &ChunkedList<T, ChunkSize, Allocator>::operator[](const size_t index) const {
    return const_cast<ChunkedList *>(this)->operator[](index);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  T &ChunkedList<T, ChunkSize, Allocator>::at(const size_t index) {
    if (index >= size()) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  const T &ChunkedList<T, ChunkSize, Allocator>::at(const size_t index) const {
    if (index >= size()) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename IteratorType>
    requires utility::iterator_or_chunk_iterator<ChunkedList<T, ChunkSize, Allocator>, IteratorType>
  IteratorType ChunkedList<T, ChunkSize, Allocator>::begin() {
    return IteratorType{get_sentinel()->nextChunk};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ConstIteratorType>
    requires utility::iterator_or_chunk_iterator_const<ChunkedList<T, ChunkSize, Allocator>, ConstIteratorType>
  ConstIteratorType ChunkedList<T, ChunkSize, Allocator>::begin() const {
    return ConstIteratorType{get_sentinel()->nextChunk};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ConstIteratorType>
    requires utility::iterator_or_chunk_iterator_const<ChunkedList<T, ChunkSize, Allocator>, ConstIteratorType>
  ConstIteratorType ChunkedList<T, ChunkSize, Allocator>::cbegin() const {
    return begin();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ReverseIteratorType>
    requires utility::iterator_or_chunk_iterator_reverse<ChunkedList<T, ChunkSize, Allocator>, ReverseIteratorType>
  ReverseIteratorType ChunkedList<T, ChunkSize, Allocator>::rbegin() {
    return ReverseIteratorType{end()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ConstReverseIteratorType>
    requires utility::iterator_or_chunk_iterator_const_reverse<ChunkedList<T, ChunkSize, Allocator>,
                                                               ConstReverseIteratorType>
  ConstReverseIteratorType ChunkedList<T, ChunkSize, Allocator>::rbegin() const {
    return ConstReverseIteratorType{cend()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ConstReverseIteratorType>
    requires utility::iterator_or_chunk_iterator_const_reverse<ChunkedList<T, ChunkSize, Allocator>,
                                                               ConstReverseIteratorType>
  ConstReverseIteratorType ChunkedList<T, ChunkSize, Allocator>::crbegin() const {
    return rbegin();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename IteratorType>
    requires utility::iterator_or_chunk_iterator<ChunkedList<T, ChunkSize, Allocator>, IteratorType>
  IteratorType ChunkedList<T, ChunkSize, Allocator>::end() {
    return IteratorType{get_sentinel()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ConstIteratorType>
    requires utility::iterator_or_chunk_iterator_const<ChunkedList<T, ChunkSize, Allocator>, ConstIteratorType>
  ConstIteratorType ChunkedList<T, ChunkSize, Allocator>::end() const {
    return ConstIteratorType{get_sentinel()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ConstIteratorType>
    requires utility::iterator_or_chunk_iterator_const<ChunkedList<T, ChunkSize, Allocator>, ConstIteratorType>
  ConstIteratorType ChunkedList<T, ChunkSize, Allocator>::cend() const {
    return end();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ReverseIteratorType>
    requires utility::iterator_or_chunk_iterator_reverse<ChunkedList<T, ChunkSize, Allocator>, ReverseIteratorType>
  ReverseIteratorType ChunkedList<T, ChunkSize, Allocator>::rend() {
    return ReverseIteratorType{begin()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ConstReverseIteratorType>
    requires utility::iterator_or_chunk_iterator_const_reverse<ChunkedList<T, ChunkSize, Allocator>,
                                                               ConstReverseIteratorType>
  ConstReverseIteratorType ChunkedList<T, ChunkSize, Allocator>::rend() const {
    return ConstReverseIteratorType{begin()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename ConstReverseIteratorType>
    requires utility::iterator_or_chunk_iterator_const_reverse<ChunkedList<T, ChunkSize, Allocator>,
                                                               ConstReverseIteratorType>
  ConstReverseIteratorType ChunkedList<T, ChunkSize, Allocator>::crend() const {
    return rend();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::mutable_slice
  ChunkedList<T, ChunkSize, Allocator>::slice(const size_t startIndex, const size_t endIndex) {
    chunk_iterator chunkIt{get_sentinel()->nextChunk};

    const size_t startChunkIndex = startIndex / ChunkSize;

    for (size_t i = 0; i < startChunkIndex; ++i) {
      ++chunkIt;
    }

    Chunk &startChunk = *chunkIt;

    const size_t endChunkIndex = endIndex / ChunkSize;

    for (size_t i = startChunkIndex; i < endChunkIndex; ++i) {
      ++chunkIt;
    }

    return mutable_slice{iterator{startChunk, startIndex % ChunkSize}, iterator{*chunkIt, endIndex % ChunkSize}};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::immutable_slice
  ChunkedList<T, ChunkSize, Allocator>::slice(const size_t startIndex, const size_t endIndex) const {
    return immutable_slice{const_cast<ChunkedList *>(this)->slice(startIndex, endIndex)};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::immutable_slice
  ChunkedList<T, ChunkSize, Allocator>::cslice(const size_t startIndex, const size_t endIndex) const {
    return slice();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::all_iterators_or_chunk_iterators<ChunkedList<T, ChunkSize, Allocator>, StartIteratorT,
                                                       EndIteratorT>
  typename ChunkedList<T, ChunkSize, Allocator>::mutable_slice
  ChunkedList<T, ChunkSize, Allocator>::slice(StartIteratorT start, EndIteratorT end) {
    return mutable_slice{start, end};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::all_iterators_or_chunk_iterators<ChunkedList<T, ChunkSize, Allocator>, StartIteratorT,
                                                       EndIteratorT>
  typename ChunkedList<T, ChunkSize, Allocator>::immutable_slice
  ChunkedList<T, ChunkSize, Allocator>::slice(StartIteratorT start, EndIteratorT end) const {
    return immutable_slice{start, end};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::all_iterators_or_chunk_iterators<ChunkedList<T, ChunkSize, Allocator>, StartIteratorT,
                                                       EndIteratorT>
  typename ChunkedList<T, ChunkSize, Allocator>::immutable_slice
  ChunkedList<T, ChunkSize, Allocator>::cslice(StartIteratorT start, EndIteratorT end) const {
    return slice();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::push_back(T &&value) {
    if (Chunk *backPtr = get_sentinel()->prevChunk; backPtr->size() == ChunkSize) {
      Chunk *chunkPtr = ChunkAllocatorTraits::allocate(chunk_allocator, 1);
      new (chunkPtr) Chunk{std::forward<T>(value), backPtr, get_sentinel()};

      backPtr->nextChunk = chunkPtr;
      get_sentinel()->prevChunk = chunkPtr;

      ++chunk_count;
    } else {
      backPtr->push_back(std::forward<T>(value));
    }
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::push_back(const T &value) {
    push_back(T{value});
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename... Args>
    requires utility::can_construct<T, Args...>
  void ChunkedList<T, ChunkSize, Allocator>::emplace_back(Args &&...args) {
    if (Chunk *backPtr = get_sentinel()->prevChunk; backPtr->size() == ChunkSize) {
      Chunk *chunkPtr = ChunkAllocatorTraits::allocate(chunk_allocator, 1);
      new (chunkPtr) Chunk{T{std::forward<Args>(args)}..., backPtr, sentinel};

      backPtr->nextChunk = chunkPtr;
      get_sentinel()->prevChunk = chunkPtr;

      ++chunk_count;
    } else {
      backPtr->emplace_back(std::forward<Args>(args)...);
    }
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::pop_back() {
    if (Chunk *backPtr = get_sentinel()->prevChunk; backPtr->empty()) {
      pop_chunk();
    } else {
      backPtr->pop_back();
    }
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::iterator ChunkedList<T, ChunkSize, Allocator>::erase(iterator it) {
    iterator nextIt = ++it;
    iterator firstIt = it, secondIt = nextIt, endIt = end();

    while (secondIt != endIt) {
      *firstIt = std::move(*secondIt);
      firstIt = secondIt;
      ++secondIt;
    }

    return nextIt;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::chunk_iterator
  ChunkedList<T, ChunkSize, Allocator>::erase(chunk_iterator iterator) {
    Chunk *prevPtr = iterator->prevChunk;
    Chunk *nextPtr = iterator->nextChunk;

    std::destroy_at<Chunk *>(iterator);
    ChunkAllocatorTraits::deallocate(chunk_allocator, iterator, 1);

    prevPtr->nextChunk = nextPtr;
    nextPtr->prevChunk = prevPtr;

    return chunk_iterator{nextPtr};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::pop_chunk() {
    Chunk *backPtr = get_sentinel()->prevChunk;
    Chunk *prevPtr = backPtr->prevChunk;

    prevPtr->nextChunk = get_sentinel();
    get_sentinel()->prevChunk = prevPtr;

    std::destroy_at(backPtr);
    ChunkAllocatorTraits::deallocate(chunk_allocator, backPtr, 1);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::clear() {
    Chunk *backPtr = get_sentinel()->prevChunk;

    while (backPtr != sentinel) {
      Chunk *prev = backPtr->prevChunk;
      std::destroy_at(backPtr);
      ChunkAllocatorTraits::deallocate(chunk_allocator, backPtr, 1);
      backPtr = prev;
    }

    chunk_count = 0;
    get_sentinel()->prevChunk = sentinel;
    get_sentinel()->nextChunk = sentinel;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
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

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  size_t ChunkedList<T, ChunkSize, Allocator>::size() const {
    return chunk_count == 0
             ? 0
             : (chunk_count - 1) * ChunkSize + reinterpret_cast<const Chunk *>(&sentinel)->prevChunk->size();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::empty() const {
    return chunk_count == 0;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::operator==(const ChunkedList &other) const {
    if (size() != other.size())
      return false;

    for (const_iterator thisIterator = cbegin(), otherIterator = other.begin(); thisIterator != cend();
         ++thisIterator, ++otherIterator)
      if (*thisIterator != *otherIterator)
        return false;

    return true;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::operator!=(const ChunkedList &other) const {
    if (size() != other.size())
      return true;

    for (const_iterator thisIterator = cbegin(), otherIterator = other.begin(); thisIterator != cend();
         ++thisIterator, ++otherIterator)
      if (*thisIterator != *otherIterator)
        return true;

    return false;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
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

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
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

    iterator it = begin(), endIt = end();

    stream << *it;
    ++it;

    while (it != endIt) {
      stream << delimiter << *it;
      ++it;
    }

    return stream.str();
  }
} // namespace chunked_list

template<chunked_list::utility::chunked_list ChunkedListType, typename IteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator<ChunkedListType, IteratorType>
IteratorType begin(ChunkedListType &chunkedList) noexcept {
  return chunkedList.template begin<IteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ConstIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_const<ChunkedListType, ConstIteratorType>
ConstIteratorType begin(const ChunkedListType &chunkedList) noexcept {
  return chunkedList.template cbegin<ConstIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ConstIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_const<ChunkedListType, ConstIteratorType>
ConstIteratorType cbegin(const ChunkedListType &chunkedList) noexcept {
  return chunkedList.template cbegin<ConstIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ReverseIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_reverse<ChunkedListType, ReverseIteratorType>
ReverseIteratorType rbegin(ChunkedListType &chunkedList) noexcept {
  return chunkedList.template rbegin<ReverseIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ConstReverseIteratorType>
ConstReverseIteratorType rbegin(const ChunkedListType &chunkedList) noexcept {
  return chunkedList.template rbegin<ConstReverseIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ConstReverseIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_const_reverse<ChunkedListType, ConstReverseIteratorType>
ConstReverseIteratorType crbegin(const ChunkedListType &chunkedList) noexcept {
  return chunkedList.template crbegin<ConstReverseIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename IteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator<ChunkedListType, IteratorType>
IteratorType end(ChunkedListType &chunkedList) noexcept {
  return chunkedList.template end<IteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ConstIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_const<ChunkedListType, ConstIteratorType>
ConstIteratorType end(const ChunkedListType &chunkedList) noexcept {
  return chunkedList.template end<ConstIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ConstIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_const<ChunkedListType, ConstIteratorType>
ConstIteratorType cend(const ChunkedListType &chunkedList) noexcept {
  return chunkedList.template cend<ConstIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ReverseIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_reverse<ChunkedListType, ReverseIteratorType>
ReverseIteratorType rend(ChunkedListType &chunkedList) noexcept {
  return chunkedList.template rend<ReverseIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ConstReverseIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_const_reverse<ChunkedListType, ConstReverseIteratorType>
ConstReverseIteratorType rend(const ChunkedListType &chunkedList) noexcept {
  return chunkedList.template crend<ConstReverseIteratorType>();
}

template<chunked_list::utility::chunked_list ChunkedListType, typename ConstReverseIteratorType>
  requires chunked_list::utility::iterator_or_chunk_iterator_const_reverse<ChunkedListType, ConstReverseIteratorType>
ConstReverseIteratorType crend(const ChunkedListType &chunkedList) noexcept {
  return chunkedList.template crend<ConstReverseIteratorType>();
}
