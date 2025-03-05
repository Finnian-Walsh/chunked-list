#pragma once

#include "ChunkedList.hpp"

template<typename T, size_t ChunkSize = 32>
class chunked_list final : ChunkedList<T, ChunkSize> {
  using derived_chunked_list = ChunkedList<T, ChunkSize>;

  public:
    using derived_chunked_list::ChunkedList;

    using chunk_iterator = typename derived_chunked_list::ChunkIterator;

    using const_chunk_iterator = typename derived_chunked_list::ConstChunkIterator;

    using iterator = typename derived_chunked_list::Iterator;

    using const_iterator = typename derived_chunked_list::ConstIterator;

    using derived_chunked_list::operator[];

    using derived_chunked_list::begin;

    using derived_chunked_list::end;

    chunk_iterator begin_chunk();

    const_chunk_iterator begin_chunk() const;

    chunk_iterator end_chunk();

    const_chunk_iterator end_chunk() const;

    using derived_chunked_list::push;

    using derived_chunked_list::emplace;

    using derived_chunked_list::pop;

    /**
     * @brief Pops the back (most recent) Chunk from the ChunkedList
     */
    void pop_chunk();

    using derived_chunked_list::sort;

    using derived_chunked_list::size;

    using derived_chunked_list::empty;

    bool operator==(const chunked_list &other) const;

    bool operator!=(const chunked_list &other) const;

    using derived_chunked_list::concat;

    template<typename, size_t, bool>
    friend std::ostream &operator<<(std::ostream &os, chunked_list &chunkedList);
};

#include "internal/ChunkedListSnake.tpp"
