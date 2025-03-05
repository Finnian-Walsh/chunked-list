#pragma once
#include "ChunkedList.hpp"

// ---------------------------------------------------------------------------------------------------------------------
// GenericIterator implementation
// ---------------------------------------------------------------------------------------------------------------------

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::GenericIterator(ChunkT *chunkPtr, const size_t index)
  : chunkIterator{chunkPtr}, index(index) {
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::GenericIterator(ChunkT &chunkRef, const size_t index)
  : chunkIterator(&chunkRef), index(index) {
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::GenericIterator(
  ChunkIteratorT chunkIterator, const size_t index)
  : chunkIterator{chunkIterator}, index(index) {
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT> ChunkedList<T, ChunkSize>::GenericIterator<
  ChunkT, ValueT>::operator++() {
  if (index < ChunkSize - 1) {
    ++index;
  } else if (chunkIterator->nextChunk) {
    ++chunkIterator;
    index = 0;
  } else {
    index = ChunkSize;
  }

  return *this;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT> ChunkedList<T, ChunkSize>::GenericIterator<
  ChunkT, ValueT>::operator++(int) {
  GenericIterator original = *this;
  operator++();
  return original;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT> ChunkedList<T, ChunkSize>::GenericIterator<
  ChunkT, ValueT>::operator--() {
  if (index == 0) {
    index = ChunkSize - 1;
    --chunkIterator;
  } else {
    --index;
  }

  return *this;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT> ChunkedList<T, ChunkSize>::GenericIterator<
  ChunkT, ValueT>::operator--(int) {
  Iterator original = *this;
  operator--();
  return original;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT>
ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator+(size_t offset) {
  size_t chunkOffset = offset / ChunkSize;
  offset %= ChunkSize;

  const size_t iteratorIndex = index + offset;

  if (iteratorIndex >= ChunkSize) {
    return GenericIterator{chunkIterator + (chunkOffset + 1), iteratorIndex - ChunkSize};
  }

  return GenericIterator{chunkIterator + chunkOffset, iteratorIndex};
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT>
ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator-(size_t offset) {
  // size_t chunkOffset = offset / ChunkSize;
  // offset %= ChunkSize;
  //
  // if (offset > iterator.getIndex()) {
  //   return Iterator{iterator.getChunkIterator() - (chunkOffset + 1), ChunkSize + iterator.getIndex() - offset};
  // }
  //
  // return Iterator{iterator.getChunkIterator() - chunkOffset, iterator.getIndex() - offset};

  auto chunkIt = chunkIterator;

  while (offset > ChunkSize) {
    --chunkIt;
    offset -= ChunkSize;
  }

  if (offset > index) {
    --chunkIt;
    return GenericIterator{chunkIt, ChunkSize - offset + index};
  }

  return GenericIterator{chunkIt, index - offset};
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT>
ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator+=(const size_t offset) {
  return *this = operator+(offset);
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT>
ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator-=(const size_t offset) {
  return *this = operator-(offset);
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
template<typename IteratorT>
bool ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator==(const IteratorT other) const {
  static_assert(chunked_list_utility::is_generic_iterator<ChunkedList, IteratorT>, "IteratorT must be a GenericIterator!");
  return chunkIterator == other.getChunkIterator() && index == other.getIndex();
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
template<typename IteratorT>
bool ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator!=(const IteratorT other) const {
  static_assert(chunked_list_utility::is_generic_iterator<ChunkedList, IteratorT>, "IteratorT must be a GenericIterator!");
  return chunkIterator != other.getChunkIterator() || index != other.getIndex();
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
ValueT &ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator*() {
  return (*chunkIterator)[index];
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
const ValueT &ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator*() const {
  return (*chunkIterator)[index];
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
ValueT *ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator->() {
  return &(*chunkIterator)[index];
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
const ValueT *ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::operator->() const {
  return &(*chunkIterator)[index];
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
size_t ChunkedList<T, ChunkSize>::GenericIterator<ChunkT, ValueT>::getIndex() const {
  return index;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT, typename ValueT>
typename ChunkedList<T, ChunkSize>::template GenericIterator<ChunkT, ValueT>::ChunkIteratorT ChunkedList<T, ChunkSize>::
GenericIterator<ChunkT, ValueT>::getChunkIterator() const {
  return chunkIterator;
}

// ---------------------------------------------------------------------------------------------------------------------
// GenericChunkIterator implementation
// ---------------------------------------------------------------------------------------------------------------------

template<typename T, size_t ChunkSize>
template<typename ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::GenericChunkIterator(ChunkT *chunkPtr)
  : chunk{chunkPtr} {
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::GenericChunkIterator(ChunkT &chunkRef)
  : chunk{&chunkRef} {
}


template<typename T, size_t ChunkSize>
template<typename ChunkT>
typename ChunkedList<T, ChunkSize>::template GenericChunkIterator<ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator++() {
  chunk = chunk->nextChunk;
  return *this;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
typename ChunkedList<T, ChunkSize>::template GenericChunkIterator<ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator++(int) {
  Iterator original = *this;
  chunk = chunk->nextChunk;
  return original;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
typename ChunkedList<T, ChunkSize>::template GenericChunkIterator<ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator--() {
  chunk = chunk->prevChunk;
  return *this;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
typename ChunkedList<T, ChunkSize>::template GenericChunkIterator<ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator--(int) {
  GenericChunkIterator original = *this;
  chunk = chunk->prevChunk;
  return original;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
typename ChunkedList<T, ChunkSize>::template GenericChunkIterator<ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator+(size_t offset) const {
  ChunkT *ptr{chunk};

  for (; offset > 0; --offset)
    ptr = ptr->nextChunk;

  return GenericChunkIterator{ptr};
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
typename ChunkedList<T, ChunkSize>::template GenericChunkIterator<ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator-(size_t offset) const {
  ChunkT *ptr{chunk};

  for (; offset > 0; --offset)
    ptr = ptr->prevChunk;

  return GenericChunkIterator{ptr};
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
typename ChunkedList<T, ChunkSize>::template GenericChunkIterator<ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator+=(const size_t offset) {
  return *this = operator+(offset);
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
typename ChunkedList<T, ChunkSize>::template GenericChunkIterator<ChunkT>
ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator-=(const size_t offset) {
  return *this = operator-(offset);
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
template<typename ChunkIteratorT>
bool ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator==(const ChunkIteratorT other) const {
  static_assert(chunked_list_utility::is_generic_chunk_iterator<ChunkedList, ChunkIteratorT>, "ChunkIteratorT must be a GenericChunkIterator!");
  return chunk == &*other;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
template<typename ChunkIteratorT>
bool ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator!=(const ChunkIteratorT other) const {
  static_assert(chunked_list_utility::is_generic_chunk_iterator<ChunkedList, ChunkIteratorT>, "ChunkIteratorT must be a GenericChunkIterator!");
  return chunk != &*other;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
ChunkT &ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator*() {
  return *chunk;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
const ChunkT &ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator*() const {
  return *chunk;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
ChunkT *ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator->() {
  return chunk;
}

template<typename T, size_t ChunkSize>
template<typename ChunkT>
const ChunkT *ChunkedList<T, ChunkSize>::GenericChunkIterator<ChunkT>::operator->() const {
  return chunk;
}
