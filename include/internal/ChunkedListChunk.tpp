#pragma once

#include "internal/ChunkedListMacros.hpp"

template<typename T, size_t ChunkSize>
ChunkedList<T, ChunkSize>::Chunk::Chunk(Chunk *nextChunk, Chunk *prevChunk)
  : nextChunk(nextChunk), prevChunk(prevChunk) {
}

template<typename T, size_t ChunkSize>
ChunkedList<T, ChunkSize>::Chunk::Chunk(const T *array, const size_t size, Chunk *nextChunk,
                                        Chunk *prevChunk) : nextIndex(size),
                                                            nextChunk(nextChunk),
                                                            prevChunk(prevChunk) {
  DEBUG_LOG("range = " << size << ": ")
  for (int index = 0; index < size; ++index) {
    data[index] = array[index];
    DEBUG_LOG(array[index] << ", ")
  }

  DEBUG_LINE(true)
}

template<typename T, size_t ChunkSize>
ChunkedList<T, ChunkSize>::Chunk::Chunk(T value) : data{std::move(value)}, nextIndex{1} {
}

template<typename T, size_t ChunkSize>
typename ChunkedList<T, ChunkSize>::Chunk &
ChunkedList<T, ChunkSize>::Chunk::operator+(const size_t offset) {
  Chunk *chunk{this};

  for (size_t i = 0; i < offset; ++i)
    chunk = chunk->nextChunk;

  return *chunk;
}

template<typename T, size_t ChunkSize>
typename ChunkedList<T, ChunkSize>::Chunk &
ChunkedList<T, ChunkSize>::Chunk::operator-(const size_t offset) {
  Chunk *chunk{this};

  for (size_t i = 0; i < offset; ++i)
    chunk = chunk->nextChunk;

  return *chunk;
}

template<typename T, size_t ChunkSize>
bool ChunkedList<T, ChunkSize>::Chunk::empty() const {
  return nextIndex == 0;
}

template<typename T, size_t ChunkSize>
T &ChunkedList<T, ChunkSize>::Chunk::operator[](size_t index) {
  return data[index];
}

template<typename T, size_t ChunkSize>
const T &ChunkedList<T, ChunkSize>::Chunk::operator[](size_t index) const {
  return data[index];
}

template<typename T, size_t ChunkSize>
bool ChunkedList<T, ChunkSize>::Chunk::operator==(const Chunk &other) const {
  return this == &other;
}

template<typename T, size_t ChunkSize>
bool ChunkedList<T, ChunkSize>::Chunk::operator!=(const Chunk &other) const {
  return this == &other;
}

template<typename T, size_t ChunkSize>
void ChunkedList<T, ChunkSize>::Chunk::debugData(std::string &str) const {
  std::ostringstream oss{};
  oss << "ChunkedList<" << typeid(T).name() << ", " << std::to_string(ChunkSize) << ">::Chunk(nextIndex=" << nextIndex
  << ", nextChunk=" << nextChunk <<
  ", prevChunk = " << prevChunk;

  if (empty()) {
    goto end;
  } {
    oss << ", data=[";

    size_t index = 0;

    for (; index < nextIndex - 1; ++index) {
      oss << data[index] << ", ";
    }

    oss << data[index] << ']';
  }

end:
  oss << ')';
  str = oss.str();
}
