#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  template<typename ChunkedListT, bool Mutable>
  template<typename StartIteratorT, typename EndIteratorT>
    requires utility::are_iterators<ChunkedListT, StartIteratorT, EndIteratorT>
  ChunkedListSlice<ChunkedListT, Mutable>::ChunkedListSlice(StartIteratorT start, EndIteratorT end) :
      startIterator{start}, endIterator{end} {}

  template<typename ChunkedListT, bool Mutable>
  template<typename StartChunkIteratorT, typename EndChunkIteratorT>
    requires utility::are_chunk_iterators<ChunkedListT, StartChunkIteratorT, EndChunkIteratorT>
  ChunkedListSlice<ChunkedListT, Mutable>::ChunkedListSlice(StartChunkIteratorT start, EndChunkIteratorT last) :
      startIterator{start}, endIterator{last, last->nextIndex} {}

  template<typename ChunkedListT, bool Mutable>
  typename ChunkedListSlice<ChunkedListT, Mutable>::ValueT &
  ChunkedListSlice<ChunkedListT, Mutable>::operator[](size_t index) {
    index += startIterator.get_index();
    auto targetChunk = startIterator.get_chunk_iterator();

    while (index >= ChunkedListT::chunk_size) {
      index -= ChunkedListT::chunk_size;
      ++targetChunk;
    }

    return (*targetChunk)[index];
  }

  template<typename ChunkedListT, bool Mutable>
  const typename ChunkedListSlice<ChunkedListT, Mutable>::ValueT &
  ChunkedListSlice<ChunkedListT, Mutable>::operator[](const size_t index) const {
    return const_cast<ChunkedListSlice *>(this)->operator[](index);
  }

  template<typename ChunkedListT, bool Mutable>
  template<typename SliceT>
  bool ChunkedListSlice<ChunkedListT, Mutable>::operator==(SliceT other) const {
    return other.begin() == startIterator && other.end() == endIterator;
  }

  template<typename ChunkedListT, bool Mutable>
  template<typename SliceT>
  bool ChunkedListSlice<ChunkedListT, Mutable>::operator!=(SliceT other) const {
    return other.begin() != startIterator || other.end() != endIterator;
  }

  template<typename ChunkedListT, bool Mutable>
  typename ChunkedListSlice<ChunkedListT, Mutable>::iterator_type ChunkedListSlice<ChunkedListT, Mutable>::begin() {
    return startIterator;
  }

  template<typename ChunkedListT, bool Mutable>
  typename ChunkedListSlice<ChunkedListT, Mutable>::const_iterator_type
  ChunkedListSlice<ChunkedListT, Mutable>::cbegin() const {
    return startIterator;
  }

  template<typename ChunkedListT, bool Mutable>
  typename ChunkedListSlice<ChunkedListT, Mutable>::iterator_type ChunkedListSlice<ChunkedListT, Mutable>::end() {
    return endIterator;
  }

  template<typename ChunkedListT, bool Mutable>
  typename ChunkedListSlice<ChunkedListT, Mutable>::const_iterator_type
  ChunkedListSlice<ChunkedListT, Mutable>::cend() const {
    return endIterator;
  }
} // namespace chunked_list
