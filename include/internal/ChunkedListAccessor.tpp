#pragma once

#include "ChunkedListAccessor.hpp"

template<typename T, size_t ChunkSize>
size_t ChunkedListAccessor<T, ChunkSize>::getChunkCount() {
  return this->chunkCount;
}

template<typename T, size_t ChunkSize>
typename ChunkedListAccessor<T, ChunkSize>::Chunk *ChunkedListAccessor<T, ChunkSize>::getFront() {
  return this->front;
}

template<typename T, size_t ChunkSize>
typename ChunkedListAccessor<T, ChunkSize>::Chunk *ChunkedListAccessor<T, ChunkSize>::getBack() {
  return this->back;
}
