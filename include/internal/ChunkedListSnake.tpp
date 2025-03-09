#pragma once

#include "ChunkedListSnake.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize>
  typename Chunked_List<T, ChunkSize>::chunk_iterator Chunked_List<T,
    ChunkSize>::begin_chunk() {
    return derived_chunked_list::beginChunk();
  }

  template<typename T, size_t ChunkSize>
  typename Chunked_List<T, ChunkSize>::const_chunk_iterator Chunked_List<T,
    ChunkSize>::begin_chunk() const {
    return derived_chunked_list::beginChunk();
  }

  template<typename T, size_t ChunkSize>
  typename Chunked_List<T, ChunkSize>::chunk_iterator Chunked_List<T,
    ChunkSize>::end_chunk() {
    return derived_chunked_list::endChunk();
  }

  template<typename T, size_t ChunkSize>
  typename Chunked_List<T, ChunkSize>::const_chunk_iterator Chunked_List<T,
    ChunkSize>::end_chunk() const {
    return derived_chunked_list::endChunk();
  }

  template<typename T, size_t ChunkSize>
  void Chunked_List<T, ChunkSize>::pop_chunk() {
    derived_chunked_list::popChunk();
  }

  template<typename T, size_t ChunkSize>
  bool Chunked_List<T, ChunkSize>::operator==(const Chunked_List &other) const {
    return derived_chunked_list::operator==(other);
  }

  template<typename T, size_t ChunkSize>
  bool Chunked_List<T, ChunkSize>::operator!=(const Chunked_List &other) const {
    return derived_chunked_list::operator!=(other);
  }

  template<typename T, size_t ChunkSize>
  std::ostream &operator<<(std::ostream &os, Chunked_List<T, ChunkSize> &chunkedList) {
    return operator<<(os, *reinterpret_cast<chunked_list::ChunkedList<T, ChunkSize> *>(&chunkedList));
  }
}
