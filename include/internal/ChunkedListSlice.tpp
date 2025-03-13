#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize>
  template<typename IteratorT>
  typename ChunkedList<T, ChunkSize>::Slice ChunkedList<T, ChunkSize>::slice(IteratorT start, IteratorT end) {
    static_assert(utility::is_iterator<ChunkedList, IteratorT> || utility::is_chunk_iterator<ChunkedList, IteratorT>,
      "IteratorT must be from GenericIterator or GenericChunkIterator!");
    return Slice{start, end};
  }

  template<typename T, size_t ChunkSize>
  template<typename IteratorT>
  typename ChunkedList<T, ChunkSize>::ConstSlice ChunkedList<T, ChunkSize>::slice(IteratorT start, IteratorT end) const {
    static_assert(utility::is_iterator<ChunkedList, IteratorT>, "IteratorT must be from GenericIterator!");
    return ConstSlice{start, end};
  }


  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT,
    ValueT>::GenericSlice(IteratorT start, IteratorT end) : firstIndex{start.getIndex()},
                                                            lastIndex{(end - 1).getIndex()},
                                                            firstChunk{&*start.getChunkIterator()},
                                                            lastChunk{&*(end - 1).getChunkIterator()} {
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::GenericSlice(ChunkIteratorT start, ChunkIteratorT end)
    : firstIndex{0}, lastIndex{(end - 1)->nextIndex - 1}, firstChunk{&*start}, lastChunk{&*end} {
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  ValueT &ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::operator[](size_t index) {
    index += firstIndex;
    auto targetChunk = ChunkIterator{firstChunk};

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
  typename ChunkedList<T, ChunkSize>::template GenericSlice<ChunkT, ValueT>::IteratorT
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::begin() {
    return IteratorT{firstChunk, firstIndex};
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize>::template GenericSlice<ChunkT, ValueT>::ConstIteratorT
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::begin() const {
    return ConstIteratorT{firstChunk, firstIndex};
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize>::template GenericSlice<ChunkT, ValueT>::IteratorT
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::end() {
    return ++IteratorT{lastChunk, lastIndex};
  }

  template<typename T, size_t ChunkSize>
  template<typename ChunkT, typename ValueT>
  typename ChunkedList<T, ChunkSize>::template GenericSlice<ChunkT, ValueT>::ConstIteratorT
  ChunkedList<T, ChunkSize>::GenericSlice<ChunkT, ValueT>::end() const {
    return ++ConstIteratorT{lastChunk, lastIndex};
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
