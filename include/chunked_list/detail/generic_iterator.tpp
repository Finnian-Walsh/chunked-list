#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  // ---------------------------------------------------------------------------------------------------------------------
  // GenericChunkIterator implementation
  // ---------------------------------------------------------------------------------------------------------------------

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  template<typename ChunkIteratorT>
    requires utility::chunk_iterator<ChunkedList<T, ChunkSize, Allocator>, ChunkIteratorT>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::generic_chunk_iterator(
    ChunkIteratorT chunkIterator) : chunk{chunkIterator} {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::generic_chunk_iterator(ChunkT *chunkPtr) :
      chunk{chunkPtr} {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::generic_chunk_iterator(ChunkT &chunkRef) :
      chunk{&chunkRef} {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator++() {
    chunk = chunk->nextChunk;
    return *this;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator++(int) {
    iterator original = *this;
    chunk = chunk->nextChunk;
    return original;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator--() {
    chunk = chunk->prevChunk;
    return *this;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator--(int) {
    generic_chunk_iterator original = *this;
    chunk = chunk->prevChunk;
    return original;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator+(size_t offset) const {
    ChunkT *ptr{chunk};

    for (; offset > 0; --offset)
      ptr = ptr->nextChunk;

    return generic_chunk_iterator{ptr};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator-(size_t offset) const {
    ChunkT *ptr{chunk};

    for (; offset > 0; --offset)
      ptr = ptr->prevChunk;

    return generic_chunk_iterator{ptr};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator+=(const size_t offset) {
    return *this = operator+(offset);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator-=(const size_t offset) {
    return *this = operator-(offset);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  template<typename ChunkIteratorType>
    requires utility::chunk_iterator<ChunkedList<T, ChunkSize, Allocator>, ChunkIteratorType>
  bool ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator==(
    const ChunkIteratorType &other) const {
    return chunk == other.chunk;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  template<typename ChunkIteratorType>
    requires utility::chunk_iterator<ChunkedList<T, ChunkSize, Allocator>, ChunkIteratorType>
  bool ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator!=(
    const ChunkIteratorType &other) const {
    return chunk != other.chunk;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>::ChunkT &
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator*() {
    return *chunk;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  const typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>::ChunkT &
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator*() const {
    return *chunk;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>::ChunkT *
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator->() {
    return chunk;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  const typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>::ChunkT *
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator->() const {
    return chunk;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>::ChunkT &
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator[](const size_t n) {
    return *operator+(n);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  const typename ChunkedList<T, ChunkSize, Allocator>::template generic_chunk_iterator<Mutable>::ChunkT &
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator[](const size_t n) const {
    return *operator+(n);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator ChunkT *() {
    return chunk;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_chunk_iterator<Mutable>::operator const ChunkT *() const {
    return chunk;
  }

  // ---------------------------------------------------------------------------------------------------------------------
  // GenericIterator implementation
  // ---------------------------------------------------------------------------------------------------------------------

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  template<utility::iterator<ChunkedList<T, ChunkSize, Allocator>> IteratorType>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::generic_iterator(IteratorType iterator) :
      chunkIterator{iterator.cget_chunk_iterator()}, index{iterator.get_index()} {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::generic_iterator(ChunkT *chunkPtr,
                                                                                    const size_t index) :
      chunkIterator{chunkPtr}, index(index) {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::generic_iterator(ChunkT &chunkRef,
                                                                                    const size_t index) :
      chunkIterator(&chunkRef), index(index) {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::generic_iterator(chunk_iterator_type chunkIterator,
                                                                                    const size_t index) :
      chunkIterator{chunkIterator}, index(index) {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator++() {
    if (index < ChunkSize - 1) {
      ++index;
    } else {
      ++chunkIterator;
      index = 0;
    }

    return *this;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator++(int) {
    generic_iterator original = *this;
    operator++();
    return original;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator--() {
    if (index == 0) {
      index = ChunkSize - 1;
      --chunkIterator;
    } else {
      --index;
    }

    return *this;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator--(int) {
    iterator original = *this;
    operator--();
    return original;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator+(size_t n) {
    size_t chunkOffset = n / ChunkSize;
    n %= ChunkSize;

    const size_t iteratorIndex = index + n;

    if (iteratorIndex >= ChunkSize) {
      return generic_iterator{chunkIterator + (chunkOffset + 1), iteratorIndex - ChunkSize};
    }

    return generic_iterator{chunkIterator + chunkOffset, iteratorIndex};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator-(size_t n) {
    auto chunkIt = chunkIterator;

    while (n > ChunkSize) {
      --chunkIt;
      n -= ChunkSize;
    }

    if (n > index) {
      --chunkIt;
      return generic_iterator{chunkIt, ChunkSize - n + index};
    }

    return generic_iterator{chunkIt, index - n};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator+=(const size_t n) {
    return *this = operator+(n);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator-=(const size_t n) {
    return *this = operator-(n);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  template<typename IteratorType>
    requires utility::iterator<ChunkedList<T, ChunkSize, Allocator>, IteratorType>
  bool ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator==(const IteratorType &other) const {
    return chunkIterator == other.chunkIterator && index == other.index;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  template<typename IteratorT>
    requires utility::iterator<ChunkedList<T, ChunkSize, Allocator>, IteratorT>
  bool ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator!=(const IteratorT &other) const {
    return chunkIterator != other.chunkIterator || index != other.index;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::ValueT &
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator*() {
    return (*chunkIterator)[index];
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  const typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::ValueT &
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator*() const {
    return (*chunkIterator)[index];
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::ValueT *
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator->() {
    return &(*chunkIterator)[index];
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  const typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::ValueT *
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator->() const {
    return &(*chunkIterator)[index];
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::ValueT &
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator[](const size_t n) {
    return *operator+(n);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  const typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::ValueT &
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::operator[](const size_t n) const {
    return *operator+(n);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  size_t ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::get_index() const {
    return index;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::ChunkT &
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::get_chunk() {
    return *chunkIterator;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  const typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::ChunkT &
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::get_chunk() const {
    return *chunkIterator;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::chunk_iterator_type
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::get_chunk_iterator() {
    return chunkIterator;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<bool Mutable>
  typename ChunkedList<T, ChunkSize, Allocator>::template generic_iterator<Mutable>::const_chunk_iterator_type
  ChunkedList<T, ChunkSize, Allocator>::generic_iterator<Mutable>::cget_chunk_iterator() const {
    return const_chunk_iterator_type{*chunkIterator};
  }
} // namespace chunked_list
