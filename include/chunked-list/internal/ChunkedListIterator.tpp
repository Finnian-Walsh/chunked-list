#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  // ---------------------------------------------------------------------------------------------------------------------
  // GenericChunkIterator implementation
  // ---------------------------------------------------------------------------------------------------------------------

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  template<typename ChunkIteratorT>
    requires utility::is_chunk_iterator<ChunkedList<T, ChunkSize, Allocator>, ChunkIteratorT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::GenericChunkIterator(
    ChunkIteratorT chunkIterator) : chunk{chunkIterator} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::GenericChunkIterator(ChunkT *chunkPtr) :
      chunk{chunkPtr} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::GenericChunkIterator(ChunkT &chunkRef) :
      chunk{&chunkRef} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericChunkIterator<ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator++() {
    chunk = chunk->nextChunk;
    return *this;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericChunkIterator<ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator++(int) {
    Iterator original = *this;
    chunk = chunk->nextChunk;
    return original;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericChunkIterator<ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator--() {
    chunk = chunk->prevChunk;
    return *this;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericChunkIterator<ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator--(int) {
    GenericChunkIterator original = *this;
    chunk = chunk->prevChunk;
    return original;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericChunkIterator<ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator+(size_t offset) const {
    ChunkT *ptr{chunk};

    for (; offset > 0; --offset)
      ptr = ptr->nextChunk;

    return GenericChunkIterator{ptr};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericChunkIterator<ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator-(size_t offset) const {
    ChunkT *ptr{chunk};

    for (; offset > 0; --offset)
      ptr = ptr->prevChunk;

    return GenericChunkIterator{ptr};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericChunkIterator<ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator+=(const size_t offset) {
    return *this = operator+(offset);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericChunkIterator<ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator-=(const size_t offset) {
    return *this = operator-(offset);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  template<typename ChunkIteratorT>
    requires utility::is_chunk_iterator<ChunkedList<T, ChunkSize, Allocator>, ChunkIteratorT>
  bool
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator==(const ChunkIteratorT other) const {
    return chunk == other;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  template<typename ChunkIteratorT>
    requires utility::is_chunk_iterator<ChunkedList<T, ChunkSize, Allocator>, ChunkIteratorT>
  bool
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator!=(const ChunkIteratorT other) const {
    return chunk != other;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  ChunkT &ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator*() {
    return *chunk;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  const ChunkT &ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator*() const {
    return *chunk;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  ChunkT *ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator->() {
    return chunk;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  const ChunkT *ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator->() const {
    return chunk;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator ChunkT *() {
    return chunk;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT>
  ChunkedList<T, ChunkSize, Allocator>::GenericChunkIterator<ChunkT>::operator const ChunkT *() const {
    return chunk;
  }

  // ---------------------------------------------------------------------------------------------------------------------
  // GenericIterator implementation
  // ---------------------------------------------------------------------------------------------------------------------

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  template<typename IteratorT>
    requires utility::is_iterator<ChunkedList<T, ChunkSize, Allocator>, IteratorT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::GenericIterator(IteratorT iterator) :
      chunkIterator{iterator.getChunkIterator()}, index{iterator.getIndex()} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::GenericIterator(ChunkT *chunkPtr,
                                                                                         const size_t index) :
      chunkIterator{chunkPtr}, index(index) {}

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::GenericIterator(ChunkT &chunkRef,
                                                                                         const size_t index) :
      chunkIterator(&chunkRef), index(index) {}

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::GenericIterator(ChunkIteratorT chunkIterator,
                                                                                         const size_t index) :
      chunkIterator{chunkIterator}, index(index) {}

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator++() {
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

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator++(int) {
    GenericIterator original = *this;
    operator++();
    return original;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator--() {
    if (index == 0) {
      index = ChunkSize - 1;
      --chunkIterator;
    } else {
      --index;
    }

    return *this;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator--(int) {
    Iterator original = *this;
    operator--();
    return original;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator+(size_t offset) {
    size_t chunkOffset = offset / ChunkSize;
    offset %= ChunkSize;

    const size_t iteratorIndex = index + offset;

    if (iteratorIndex >= ChunkSize) {
      return GenericIterator{chunkIterator + (chunkOffset + 1), iteratorIndex - ChunkSize};
    }

    return GenericIterator{chunkIterator + chunkOffset, iteratorIndex};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator-(size_t offset) {
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

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator+=(const size_t offset) {
    return *this = operator+(offset);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator-=(const size_t offset) {
    return *this = operator-(offset);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  template<typename IteratorT>
    requires utility::is_iterator<ChunkedList<T, ChunkSize, Allocator>, IteratorT>
  bool ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator==(const IteratorT other) const {
    return chunkIterator == other.getChunkIterator() && index == other.getIndex();
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  template<typename IteratorT>
    requires utility::is_iterator<ChunkedList<T, ChunkSize, Allocator>, IteratorT>
  bool ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator!=(const IteratorT other) const {
    return chunkIterator != other.getChunkIterator() || index != other.getIndex();
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  ValueT &ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator*() {
    return (*chunkIterator)[index];
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  const ValueT &ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator*() const {
    return (*chunkIterator)[index];
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  ValueT *ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator->() {
    return &(*chunkIterator)[index];
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  const ValueT *ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::operator->() const {
    return &(*chunkIterator)[index];
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  size_t ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::getIndex() const {
    return index;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  ChunkT &ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::getChunk() {
    return *chunkIterator;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  const ChunkT &ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::getChunk() const {
    return *chunkIterator;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>::ChunkIteratorT
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::getChunkIterator() {
    return chunkIterator;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize, Allocator>::template GenericIterator<ChunkT, ValueT>::ConstChunkIteratorT
  ChunkedList<T, ChunkSize, Allocator>::GenericIterator<ChunkT, ValueT>::getChunkIterator() const {
    return ConstChunkIteratorT{*chunkIterator};
  }
} // namespace chunked_list
