#pragma once

#include "../ChunkedListSnake.hpp"

namespace chunked_list {
  template<typename T, size_t Chunk_Size, typename Allocator>
  typename Chunked_List<T, Chunk_Size, Allocator>::chunk_iterator Chunked_List<T, Chunk_Size, Allocator>::begin_chunk() {
    return derived_chunked_list::beginChunk();
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  typename Chunked_List<T, Chunk_Size, Allocator>::const_chunk_iterator Chunked_List<T, Chunk_Size, Allocator>::begin_chunk() const {
    return derived_chunked_list::beginChunk();
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  typename Chunked_List<T, Chunk_Size, Allocator>::chunk_iterator Chunked_List<T, Chunk_Size, Allocator>::end_chunk() {
    return derived_chunked_list::endChunk();
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  typename Chunked_List<T, Chunk_Size, Allocator>::const_chunk_iterator Chunked_List<T, Chunk_Size, Allocator>::end_chunk() const {
    return derived_chunked_list::endChunk();
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  typename Chunked_List<T, Chunk_Size, Allocator>::slice Chunked_List<T, Chunk_Size, Allocator>::get_slice(size_t start_index,
                                                                                     size_t end_index) {
    return derived_chunked_list::slice(start_index, end_index);
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  typename Chunked_List<T, Chunk_Size, Allocator>::const_slice Chunked_List<T, Chunk_Size, Allocator>::get_slice(size_t start_index,
                                                                                           size_t end_index) const {
    return derived_chunked_list::slice(start_index, end_index);
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  template<typename Begin_Iterator_Type, typename End_Iterator_Type>
    requires utility::are_iterators_or_chunk_iterators<ChunkedList<T, Chunk_Size>, Begin_Iterator_Type,
                                                       End_Iterator_Type>
  typename Chunked_List<T, Chunk_Size, Allocator>::slice Chunked_List<T, Chunk_Size, Allocator>::get_slice(Begin_Iterator_Type begin,
                                                                                     End_Iterator_Type end) {
    return derived_chunked_list::template slice<Begin_Iterator_Type, End_Iterator_Type>(begin, end);
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  template<typename Begin_Iterator_Type, typename End_Iterator_Type>
    requires utility::are_iterators_or_chunk_iterators<ChunkedList<T, Chunk_Size>, Begin_Iterator_Type,
                                                       End_Iterator_Type>
  typename Chunked_List<T, Chunk_Size, Allocator>::const_slice
  Chunked_List<T, Chunk_Size, Allocator>::get_slice(Begin_Iterator_Type begin, End_Iterator_Type end) const {
    return derived_chunked_list::template slice<Begin_Iterator_Type, End_Iterator_Type>(begin, end);
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  void Chunked_List<T, Chunk_Size, Allocator>::pop_chunk() {
    derived_chunked_list::popChunk();
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  bool Chunked_List<T, Chunk_Size, Allocator>::operator==(const Chunked_List &other) const {
    return derived_chunked_list::operator==(other);
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  bool Chunked_List<T, Chunk_Size, Allocator>::operator!=(const Chunked_List &other) const {
    return derived_chunked_list::operator!=(other);
  }

  template<typename T, size_t Chunk_Size, typename Allocator>
  std::ostream &operator<<(std::ostream &os, Chunked_List<T, Chunk_Size, Allocator> &chunkedList) {
    return operator<<(os, *reinterpret_cast<ChunkedList<T, Chunk_Size> *>(&chunkedList));
  }
} // namespace chunked_list

template<typename T, size_t Chunk_Size, typename Allocator>
typename chunked_list::Chunked_List<T, Chunk_Size, Allocator>::iterator
begin(chunked_list::Chunked_List<T, Chunk_Size, Allocator> &chunked_list) {
  return chunked_list.begin();
}

template<typename T, size_t Chunk_Size, typename Allocator>
typename chunked_list::Chunked_List<T, Chunk_Size, Allocator>::const_iterator
begin(const chunked_list::Chunked_List<T, Chunk_Size, Allocator> &chunked_list) {
  return chunked_list.begin();
}

template<typename T, size_t Chunk_Size, typename Allocator>
typename chunked_list::Chunked_List<T, Chunk_Size, Allocator>::iterator
end(chunked_list::Chunked_List<T, Chunk_Size, Allocator> &chunked_list) {
  return chunked_list.end();
}

template<typename T, size_t Chunk_Size, typename Allocator>
typename chunked_list::Chunked_List<T, Chunk_Size, Allocator>::const_iterator
end(const chunked_list::Chunked_List<T, Chunk_Size, Allocator> &chunked_list) {
  return chunked_list.end();
}
