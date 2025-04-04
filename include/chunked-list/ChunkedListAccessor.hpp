#pragma once

#include "ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize, typename Allocator>
  class ChunkedListAccessor final : public ChunkedList<T, ChunkSize, Allocator> {
      using DerivedChunkedList = ChunkedList<T, ChunkSize, Allocator>;

    public:
      using Chunk = typename DerivedChunkedList::Chunk;

      template<typename ChunkT, typename ValueT>
      using GenericIterator = typename DerivedChunkedList::template GenericIterator<ChunkT, ValueT>;

      using DerivedChunkedList::pushChunk;

      using DerivedChunkedList::chunkCount;

      using DerivedChunkedList::front;

      using DerivedChunkedList::back;
  };

  template<typename ChunkedListT>
  using Accessor = ChunkedListAccessor<typename ChunkedListT::value_type, ChunkedListT::chunk_size,
                                       typename ChunkedListT::allocator_type>;
} // namespace chunked_list
