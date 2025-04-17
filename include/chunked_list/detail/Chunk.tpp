#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::Chunk::Chunk(Chunk *prevChunk, Chunk *nextChunk) :
      prevChunk{prevChunk}, nextChunk{nextChunk} {}

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::Chunk::Chunk(const T *pointer, const size_t size, Chunk *prevChunk,
                                                     Chunk *nextChunk) :
      nextIndex{size}, prevChunk{prevChunk}, nextChunk{nextChunk} {
    for (size_t index = 0; index < size; ++index) {
      new (array + index) T{pointer[index]};
    }
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::Chunk::Chunk(T &&value, Chunk *prevChunk, Chunk *nextChunk) :
      nextIndex{1}, prevChunk{prevChunk}, nextChunk{nextChunk} {
    new (array) T{std::forward<T>(value)};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  ChunkedList<T, ChunkSize, Allocator>::Chunk::~Chunk() {
    clear();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  T *ChunkedList<T, ChunkSize, Allocator>::Chunk::data() {
    return reinterpret_cast<T *>(array);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  const T *ChunkedList<T, ChunkSize, Allocator>::Chunk::data() const {
    return reinterpret_cast<T *>(array);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::Chunk::push_back(T &&value) {
    new (array + nextIndex) T{std::forward<T>(value)};
    ++nextIndex;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  template<typename... Args>
    requires utility::can_construct<T, Args...>
  void ChunkedList<T, ChunkSize, Allocator>::Chunk::emplace_back(Args &&...args) {
    new (array + nextIndex) T{std::forward<Args>(args)...};
    ++nextIndex;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::Chunk::pop_back() {
    --nextIndex;
    operator[](nextIndex).~T();
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  void ChunkedList<T, ChunkSize, Allocator>::Chunk::clear() {
    while (nextIndex) {
      pop_back();
    }
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Chunk &
  ChunkedList<T, ChunkSize, Allocator>::Chunk::operator+(size_t n) {
    Chunk *chunk{this};

    for (; n > 0; --n)
      chunk = chunk->nextChunk;

    return *chunk;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::Chunk &
  ChunkedList<T, ChunkSize, Allocator>::Chunk::operator-(const size_t n) {
    Chunk *chunkPtr{this};

    for (size_t i = 0; i < n; ++i) {
      chunkPtr = chunkPtr->prevChunk;
    }

    return *chunkPtr;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  size_t ChunkedList<T, ChunkSize, Allocator>::Chunk::size() const {
    return nextIndex;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::Chunk::empty() const {
    return nextIndex == 0;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  T &ChunkedList<T, ChunkSize, Allocator>::Chunk::operator[](const size_t index) {
    return *reinterpret_cast<T *>(array + index);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  const T &ChunkedList<T, ChunkSize, Allocator>::Chunk::operator[](const size_t index) const {
    return *std::launder(reinterpret_cast<const T *>(array + index));
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  T &ChunkedList<T, ChunkSize, Allocator>::Chunk::at(const size_t index) {
    if (index >= nextIndex) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  const T &ChunkedList<T, ChunkSize, Allocator>::Chunk::at(const size_t index) const {
    if (index >= nextIndex) {
      throw BoundaryError{utility::concatenate("Index ", index, " is out of bounds!")};
    }
    return operator[](index);
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::Chunk::operator==(const Chunk &other) const {
    if (other.size() != size()) {
      return false;
    }

    for (size_t i = 0; i < nextIndex; ++i) {
      if (operator[](i) != other[i]) {
        return false;
      }
    }

    return true;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  bool ChunkedList<T, ChunkSize, Allocator>::Chunk::operator!=(const Chunk &other) const {
    return this != &other;
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::iterator ChunkedList<T, ChunkSize, Allocator>::Chunk::begin() {
    return iterator{this};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::const_iterator
  ChunkedList<T, ChunkSize, Allocator>::Chunk::cbegin() const {
    return const_iterator{this};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::reverse_iterator
  ChunkedList<T, ChunkSize, Allocator>::Chunk::rbegin() {
    return reverse_iterator{end()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::const_reverse_iterator
  ChunkedList<T, ChunkSize, Allocator>::Chunk::crbegin() const {
    return const_reverse_iterator{cend()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::iterator ChunkedList<T, ChunkSize, Allocator>::Chunk::end() {
    return iterator{nextChunk};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::const_iterator
  ChunkedList<T, ChunkSize, Allocator>::Chunk::cend() const {
    return const_iterator{nextChunk};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::reverse_iterator ChunkedList<T, ChunkSize, Allocator>::Chunk::rend() {
    return reverse_iterator{begin()};
  }

  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  typename ChunkedList<T, ChunkSize, Allocator>::const_reverse_iterator
  ChunkedList<T, ChunkSize, Allocator>::Chunk::crend() const {
    return const_reverse_iterator{cbegin()};
  }

} // namespace chunked_list
