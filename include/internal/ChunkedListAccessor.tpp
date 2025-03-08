#pragma once

#include "ChunkedListAccessor.hpp"

template<typename T, size_t ChunkSize>
size_t chunked_list::ChunkedListAccessor<T, ChunkSize>::getChunkCount() {
  return this->chunkCount;
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedListAccessor<T, ChunkSize>::Chunk *chunked_list::ChunkedListAccessor<T, ChunkSize>::getFront() {
  return this->front;
}

template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedListAccessor<T, ChunkSize>::Chunk *chunked_list::ChunkedListAccessor<T, ChunkSize>::getBack() {
  return this->back;
}
