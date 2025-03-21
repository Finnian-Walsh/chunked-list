#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::are_iterators<ChunkedList<T, ChunkSize>, StartIteratorT, EndIteratorT>
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::GenericSlice(StartIteratorT start, EndIteratorT end)
    : startIterator{start}, endIterator{end} {
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  template<typename StartChunkIteratorT, typename EndChunkIteratorT>
    requires utility::are_chunk_iterators<ChunkedList<T, ChunkSize>, StartChunkIteratorT, EndChunkIteratorT>
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::GenericSlice(StartChunkIteratorT start, EndChunkIteratorT last)
    : startIterator{start}, endIterator{last, last->nextIndex} {}

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  ValueT &ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::operator[](size_t index) {
    index += startIterator.getIndex();
    auto targetChunk = startIterator.getChunkIterator();

    while (index >= ChunkSize) {
      index -= ChunkSize;
      ++targetChunk;
    }

    return (*targetChunk)[index];
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  const ValueT &ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::operator[](const size_t index) const {
    return const_cast<GenericSlice *>(this)->operator[](index);
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  template<typename SliceT>
    requires utility::is_slice<ChunkedList<T, ChunkSize>, SliceT>
  bool ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::operator==(SliceT other) const {
    return other.begin() == startIterator && other.end() == endIterator;
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  template<typename SliceT>
    requires utility::is_slice<ChunkedList<T, ChunkSize>, SliceT>
  bool ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::operator!=(SliceT other) const {
    return other.begin() != startIterator || other.end() != endIterator;
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize>::template GenericSlice<ChunkT, ValueT>::IteratorType
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::begin() {
    return startIterator;
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize>::template GenericSlice<ChunkT, ValueT>::ConstIteratorType
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::begin() const {
    return startIterator;
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize>::template GenericSlice<ChunkT, ValueT>::IteratorType
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::end() {
    return endIterator;
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize>::template GenericSlice<ChunkT, ValueT>::ConstIteratorType
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::end() const {
    return endIterator;
  }

  // template<typename T, size_t ChunkSize>
  // template<typename ChunkT, typename ValueT>
  // void ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::expandLeft(const size_t distance) {
  //   // implement this
  // }
  //
  // template<typename T, size_t ChunkSize>
  // template<typename ChunkT, typename ValueT>
  // void ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::expandRight(const size_t distance) {
  //   // implement this
  // }
}
