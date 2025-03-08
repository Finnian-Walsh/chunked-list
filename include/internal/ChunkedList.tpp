#pragma once

#include <iostream>

#include "ChunkedList.hpp"
#include "internal/ChunkedListUtility.hpp"

template<typename T, size_t ChunkSize>
void chunked_list::ChunkedList<T, ChunkSize>::pushChunk(Chunk *chunk) {
  back->nextChunk = chunk;
  chunk->prevChunk = back;
  back = chunk;
}

template<typename T, size_t ChunkSize>
chunked_list::ChunkedList<T, ChunkSize>::ChunkedList() {
  front = back = new Chunk{};
}

template<typename T, size_t ChunkSize>
chunked_list::ChunkedList<T, ChunkSize>::ChunkedList(std::initializer_list<T> initializerList) {
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

  for (int offset = 1; offset < chunkCount - 1; ++offset) {
    pushChunk(new Chunk{initializerList.begin() + (offset * ChunkSize), ChunkSize});
  }

  const int remainingItems = initializerList.size() % ChunkSize;
  pushChunk(
    new Chunk{
      initializerList.begin() + (chunkCount - 1) * ChunkSize, remainingItems == 0 ? ChunkSize : remainingItems
    });
}

template<typename T, size_t ChunkSize>
chunked_list::ChunkedList<T, ChunkSize>::~ChunkedList() {
  do {
    Chunk *newBack = back->prevChunk;
    delete back;
    back = newBack;
  } while (back);
}

template<typename T, size_t ChunkSize>
T &chunked_list::ChunkedList<T, ChunkSize>::operator[](const size_t index) {
  size_t chunkIndex = index / ChunkSize;

  Chunk *chunk = front;

  while (chunkIndex-- > 0) {
    chunk = chunk->nextChunk;
  }

  return (*chunk)[index % ChunkSize];
}

template<typename T, size_t ChunkSize>
const T &chunked_list::ChunkedList<T, ChunkSize>::operator[](const size_t index) const {
  return const_cast<ChunkedList *>(this)->operator[](index);
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator chunked_list::ChunkedList<T, ChunkSize>::begin() {
  return Iterator{front, 0};
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator chunked_list::ChunkedList<T, ChunkSize>::begin() const {
  return ConstIterator{front, 0};
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator chunked_list::ChunkedList<T, ChunkSize>::end() {
  return Iterator{back, back->nextIndex};
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator chunked_list::ChunkedList<T, ChunkSize>::end() const {
  return ConstIterator{back, back->nextIndex};
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ChunkIterator chunked_list::ChunkedList<T, ChunkSize>::beginChunk() {
  return ChunkIterator{front};
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstChunkIterator chunked_list::ChunkedList<T, ChunkSize>::beginChunk() const {
  return ConstChunkIterator{front};
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ChunkIterator chunked_list::ChunkedList<T, ChunkSize>::endChunk() {
  return ChunkIterator{nullptr};
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstChunkIterator chunked_list::ChunkedList<T, ChunkSize>::endChunk() const {
  return ConstChunkIterator{nullptr};
}

template<typename T, size_t ChunkSize>
void chunked_list::ChunkedList<T, ChunkSize>::push(T value) {
  if (back->nextIndex == ChunkSize) {
    pushChunk(new Chunk{std::forward<T>(value)});
    ++chunkCount;
  } else {
    (*back)[back->nextIndex] = std::forward<T>(value);
    ++back->nextIndex;
  }
}

template<typename T, size_t ChunkSize>
template<typename... Args>
void chunked_list::ChunkedList<T, ChunkSize>::emplace(Args &&... args) {
  if (back->nextIndex == ChunkSize) {
    pushChunk(new Chunk{T(std::forward<Args>(args)...)});
  } else {
    (*back)[back->nextIndex] = std::move(T(std::forward<Args>(args)...));
    ++back->nextIndex;
  }
}

template<typename T, size_t ChunkSize>
void chunked_list::ChunkedList<T, ChunkSize>::pop() {
  if (back->nextIndex == 0)
    popChunk();
  else
    --back->nextIndex;
}

template<typename T, size_t ChunkSize>
void chunked_list::ChunkedList<T, ChunkSize>::popChunk() {
  if (Chunk *newBack = back->prevChunk; newBack) {
    delete back;
    back = newBack;
    --chunkCount;
    --back->nextIndex;
  }
}

template<typename T, size_t ChunkSize>
template<typename Compare, chunked_list::utility::SortType Sort>
void chunked_list::ChunkedList<T, ChunkSize>::sort() {
  using namespace utility;
  using namespace sort_functions;

  switch (Sort) {
    case BubbleSort: return bubble_sort<Compare>(*this);
    case SelectionSort: return selection_sort<Compare>(*this);
    case InsertionSort: return insertion_sort<Compare>(*this);
    case QuickSort: return quick_sort<Compare, T, ChunkSize>(begin(), end());
    case HeapSort: return heap_sort<Compare>(*this);
  }
}

template<typename T, size_t ChunkSize>
size_t chunked_list::ChunkedList<T, ChunkSize>::size() const {
  return (chunkCount - 1) * ChunkSize + back->nextIndex;
}

template<typename T, size_t ChunkSize>
bool chunked_list::ChunkedList<T, ChunkSize>::empty() const {
  return back->nextIndex == 0;
}

template<typename T, size_t ChunkSize>
bool chunked_list::ChunkedList<T, ChunkSize>::operator==(const ChunkedList &other) const {
  if (size() != other.size())
    return false;

  for (ConstIterator thisIterator = begin(), otherIterator = other.begin();
       thisIterator != end(); ++thisIterator, ++otherIterator)
    if (*thisIterator != *otherIterator)
      return false;

  return true;
}

template<typename T, size_t ChunkSize>
bool chunked_list::ChunkedList<T, ChunkSize>::operator!=(const ChunkedList &other) const {
  if (size() != other.size())
    return true;

  for (ConstIterator thisIterator = begin(), otherIterator = other.begin();
       thisIterator != end(); ++thisIterator, ++otherIterator)
    if (*thisIterator != *otherIterator)
      return true;

  return false;
}

template<typename T, size_t ChunkSize>
std::ostream &operator<<(std::ostream &os, chunked_list::ChunkedList<T, ChunkSize> &chunkedList) {
  os << '[';

  auto iterator = chunkedList.begin(), lastIterator = chunkedList.end() - 1;

  for (; iterator != lastIterator; ++iterator) {
    os << *iterator << ", ";
  }

  os << *lastIterator;

  os << "]";

  return os;
}

template<typename T, size_t ChunkSize>
template<typename OutputStream, typename DelimiterType>
auto chunked_list::ChunkedList<T, ChunkSize>::concat(const DelimiterType delimiter) -> decltype(std::declval<OutputStream>().str()) {
  using StringType = DeduceStringType<OutputStream>;

  static_assert(utility::can_insert<OutputStream, T>, "OutputStream cannot handle StringType");
  static_assert(utility::can_insert<OutputStream, DelimiterType>,
                "OutputStream cannot handle DelimiterType");

  if (empty()) {
    return StringType{};
  }

  OutputStream stream;

  Iterator it = begin(), lastIt = end() - 1;

  for (; it != lastIt; ++it) {
    stream << *it << delimiter;
  }

  stream << *lastIt;

  return stream.str();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator begin(
  chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept {
  return chunkedList.begin();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator begin(
  const chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept {
  return chunkedList.begin();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator end(
  chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept {
  return chunkedList.end();
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator end(
  const chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept {
  return chunkedList.end();
}
