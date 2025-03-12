#pragma once

#include "../ChunkedListSnake.hpp"

namespace chunked_list {
  template<typename T, size_t Chunk_Size>
  typename Chunked_List<T, Chunk_Size>::chunk_iterator
  Chunked_List<T, Chunk_Size>::begin_chunk() {
    return derived_chunked_list::beginChunk();
  }

  template<typename T, size_t Chunk_Size>
  typename Chunked_List<T, Chunk_Size>::const_chunk_iterator Chunked_List<T, Chunk_Size>::begin_chunk() const {
    return derived_chunked_list::beginChunk();
  }

  template<typename T, size_t Chunk_Size>
  typename Chunked_List<T, Chunk_Size>::chunk_iterator
  Chunked_List<T, Chunk_Size>::end_chunk() {
    return derived_chunked_list::endChunk();
  }

  template<typename T, size_t Chunk_Size>
  typename Chunked_List<T, Chunk_Size>::const_chunk_iterator
  Chunked_List<T, Chunk_Size>::end_chunk() const {
    return derived_chunked_list::endChunk();
  }

  template<typename T, size_t Chunk_Size>
  template<typename Iterator_Type>
  typename Chunked_List<T, Chunk_Size>::slice
  Chunked_List<T, Chunk_Size>::get_slice(Iterator_Type begin, Iterator_Type end) {
    return derived_chunked_list::template slice<Iterator_Type>(begin, end);
  }

  template<typename T, size_t Chunk_Size>
  template<typename Iterator_Type>
  typename Chunked_List<T, Chunk_Size>::const_slice
  Chunked_List<T, Chunk_Size>::get_slice(Iterator_Type begin, Iterator_Type end) const {
    return derived_chunked_list::template slice<Iterator_Type>(begin, end);
  }

  template<typename T, size_t Chunk_Size>
  void Chunked_List<T, Chunk_Size>::pop_chunk() {
    derived_chunked_list::popChunk();
  }

  template<typename T, size_t Chunk_Size>
  bool Chunked_List<T, Chunk_Size>::operator==(const Chunked_List &other) const {
    return derived_chunked_list::operator==(other);
  }

  template<typename T, size_t Chunk_Size>
  bool Chunked_List<T, Chunk_Size>::operator!=(const Chunked_List &other) const {
    return derived_chunked_list::operator!=(other);
  }

  template<typename T, size_t Chunk_Size>
  std::ostream &operator<<(std::ostream &os, Chunked_List<T, Chunk_Size> &chunkedList) {
    return operator<<(os, *reinterpret_cast<ChunkedList<T, Chunk_Size> *>(&chunkedList));
  }
}

template<typename T, size_t Chunk_Size>
typename chunked_list::Chunked_List<T, Chunk_Size>::iterator
begin(chunked_list::Chunked_List<T, Chunk_Size> &chunked_list) {
  return chunked_list.begin();
}

template<typename T, size_t Chunk_Size>
typename chunked_list::Chunked_List<T, Chunk_Size>::const_iterator
begin(const chunked_list::Chunked_List<T, Chunk_Size> &chunked_list) {
  return chunked_list.begin();
}

template<typename T, size_t Chunk_Size>
typename chunked_list::Chunked_List<T, Chunk_Size>::iterator
end(chunked_list::Chunked_List<T, Chunk_Size> &chunked_list) {
  return chunked_list.end();
}

template<typename T, size_t Chunk_Size>
typename chunked_list::Chunked_List<T, Chunk_Size>::const_iterator
end(const chunked_list::Chunked_List<T, Chunk_Size> &chunked_list) {
  return chunked_list.end();
}