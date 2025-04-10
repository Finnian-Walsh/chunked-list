#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::Chunk::Chunk(Chunk *prevChunk, Chunk *nextChunk) :
      nextChunk{nextChunk}, prevChunk{prevChunk} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::Chunk::Chunk(const T *array, const size_t size, Chunk *prevChunk,
                                                     Chunk *nextChunk) :
      nextIndex{size}, nextChunk{nextChunk}, prevChunk{prevChunk} {
    for (size_t index = 0; index < size; ++index) {
      data[index] = std::move(array[index]);
    }
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::Chunk::Chunk(T value, Chunk *prevChunk, Chunk *nextChunk) :
      data{std::move(value)}, nextIndex{1}, nextChunk{nextChunk}, prevChunk{prevChunk} {}

  template<typename T, size_t ChunkSize, typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::Chunk::push(T value) {
    data[nextIndex] = std::move(value);
    ++nextIndex;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<typename... Args>
    requires utility::can_construct<T, Args...>
  void ChunkedList<T, ChunkSize, Allocator>::Chunk::emplace(Args &&...args) {
    data[nextIndex] = std::move(T{args...});
    ++nextIndex;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<bool DestroyValue>
  void ChunkedList<T, ChunkSize, Allocator>::Chunk::pop() {
    --nextIndex;

    if constexpr (DestroyValue) {
      T *value = data + nextIndex;
      AllocatorTraits::destroy(valueAllocator, value);
      AllocatorTraits::construct(valueAllocator, value);
    }
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  template<bool DestroyData>
  void ChunkedList<T, ChunkSize, Allocator>::Chunk::clear() {
    if constexpr (DestroyData) {
      ArrayAllocatorTraits::destroy(arrayAllocator, data);
      ArrayAllocatorTraits::construct(arrayAllocator, data);
    }

    nextIndex = 0;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Chunk &
  ChunkedList<T, ChunkSize, Allocator>::Chunk::operator+(size_t offset) {
    Chunk *chunk{this};

    for (; offset > 0; --offset)
      chunk = chunk->nextChunk;

    return *chunk;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Chunk &
  ChunkedList<T, ChunkSize, Allocator>::Chunk::operator-(const size_t offset) {
    Chunk *chunk{this};

    for (size_t i = 0; i < offset; ++i) {
      chunk = chunk->prevChunk;
    }

    return *chunk;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  size_t ChunkedList<T, ChunkSize, Allocator>::Chunk::size() const {
    return nextIndex;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::Chunk::empty() const {
    return nextIndex == 0;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  T &ChunkedList<T, ChunkSize, Allocator>::Chunk::operator[](const size_t index) {
    return data[index];
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  const T &ChunkedList<T, ChunkSize, Allocator>::Chunk::operator[](const size_t index) const {
    return data[index];
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  T &ChunkedList<T, ChunkSize, Allocator>::Chunk::at(const size_t index) {
    if (index >= nextIndex) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  const T &ChunkedList<T, ChunkSize, Allocator>::Chunk::at(const size_t index) const {
    if (index >= nextIndex) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::Chunk::operator==(const Chunk &other) const {
    return this == &other;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::Chunk::operator!=(const Chunk &other) const {
    return this != &other;
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Chunk::Iterator ChunkedList<T, ChunkSize, Allocator>::Chunk::begin() {
    return Iterator{this, 0};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Chunk::ConstIterator
  ChunkedList<T, ChunkSize, Allocator>::Chunk::begin() const {
    return ConstIterator{this, 0};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Chunk::Iterator ChunkedList<T, ChunkSize, Allocator>::Chunk::end() {
    return Iterator{this, nextIndex};
  }

  template<typename T, size_t ChunkSize, typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Chunk::ConstIterator
  ChunkedList<T, ChunkSize, Allocator>::Chunk::end() const {
    return ConstIterator{this, nextIndex};
  }

} // namespace chunked_list
