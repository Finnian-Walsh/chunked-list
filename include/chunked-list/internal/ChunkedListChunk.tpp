#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::Chunk::Chunk(Chunk *prevChunk, Chunk *nextChunk) :
      nextChunk{nextChunk}, prevChunk{prevChunk} {}

  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::Chunk::Chunk(const T *array, const size_t size, Chunk *prevChunk, Chunk *nextChunk) :
      nextIndex{size}, nextChunk{nextChunk}, prevChunk{prevChunk} {
    for (size_t index = 0; index < size; ++index) {
      data[index] = std::move(array[index]);
    }
  }

  template<typename T, size_t ChunkSize>
  ChunkedList<T, ChunkSize>::Chunk::Chunk(T value, Chunk *prevChunk, Chunk *nextChunk) :
      data{std::move(value)}, nextIndex{1}, nextChunk{nextChunk}, prevChunk{prevChunk} {}

  template<typename T, size_t ChunkSize>
  void ChunkedList<T, ChunkSize>::Chunk::push(T value) {
    data[nextIndex] = std::move(value);
    ++nextIndex;
  }

  template<typename T, size_t ChunkSize>
  template<typename... Args>
    requires utility::can_construct<T, Args...>
  void ChunkedList<T, ChunkSize>::Chunk::emplace(Args &&...args) {
    data[nextIndex] = T{args...};
    ++nextIndex;
  }

  template<typename T, size_t ChunkSize>
  void ChunkedList<T, ChunkSize>::Chunk::pop() {
    --nextIndex;
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Chunk &ChunkedList<T, ChunkSize>::Chunk::operator+(size_t offset) {
    Chunk *chunk{this};

    for (; offset > 0; --offset)
      chunk = chunk->nextChunk;

    return *chunk;
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Chunk &ChunkedList<T, ChunkSize>::Chunk::operator-(const size_t offset) {
    Chunk *chunk{this};

    for (size_t i = 0; i < offset; ++i) {
      chunk = chunk->prevChunk;
    }

    return *chunk;
  }

  template<typename T, size_t ChunkSize>
  size_t ChunkedList<T, ChunkSize>::Chunk::size() const {
    return nextIndex;
  }

  template<typename T, size_t ChunkSize>
  bool ChunkedList<T, ChunkSize>::Chunk::empty() const {
    return nextIndex == 0;
  }

  template<typename T, size_t ChunkSize>
  T &ChunkedList<T, ChunkSize>::Chunk::operator[](const size_t index) {
    return data[index];
  }

  template<typename T, size_t ChunkSize>
  const T &ChunkedList<T, ChunkSize>::Chunk::operator[](const size_t index) const {
    return data[index];
  }

  template<typename T, size_t ChunkSize>
  T &ChunkedList<T, ChunkSize>::Chunk::at(const size_t index) {
    if (index >= nextIndex) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize>
  const T &ChunkedList<T, ChunkSize>::Chunk::at(const size_t index) const {
    if (index >= nextIndex) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize>
  bool ChunkedList<T, ChunkSize>::Chunk::operator==(const Chunk &other) const {
    return this == &other;
  }

  template<typename T, size_t ChunkSize>
  bool ChunkedList<T, ChunkSize>::Chunk::operator!=(const Chunk &other) const {
    return this != &other;
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Chunk::Iterator ChunkedList<T, ChunkSize>::Chunk::begin() {
    return Iterator{this, 0};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Chunk::ConstIterator ChunkedList<T, ChunkSize>::Chunk::begin() const {
    return Iterator{this, 0};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Chunk::Iterator ChunkedList<T, ChunkSize>::Chunk::end() {
    return Iterator{this, nextIndex};
  }

  template<typename T, size_t ChunkSize>
  typename ChunkedList<T, ChunkSize>::Chunk::ConstIterator ChunkedList<T, ChunkSize>::Chunk::end() const {
    return Iterator{this, nextIndex};
  }

  template<typename T, size_t ChunkSize>
  void ChunkedList<T, ChunkSize>::Chunk::getData(std::string &str) const {
    std::ostringstream oss{};
    oss << "ChunkedList<" << typeid(T).name() << ", " << std::to_string(ChunkSize) << ">::Chunk(nextIndex=" << nextIndex
        << ", nextChunk=" << nextChunk << ", prevChunk = " << prevChunk;

    if (empty()) {
      goto end;
    }
    {
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
} // namespace chunked_list
