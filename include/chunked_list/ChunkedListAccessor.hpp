#pragma once

#include "ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize, typename Allocator>
  class ChunkedListAccessor : public ChunkedList<T, ChunkSize, Allocator> {
    protected:
      using DerivedChunkedList = ChunkedList<T, ChunkSize, Allocator>;

    public:
      using DerivedChunkedList::chunkCount;

      using DerivedChunkedList::AllocatorTraits;

      using DerivedChunkedList::Chunk;

      using DerivedChunkedList::ChunkAllocator;

      using DerivedChunkedList::ChunkAllocatorTraits;

      using DerivedChunkedList::chunkAllocator;

      using DerivedChunkedList::front;

      using DerivedChunkedList::back;

      using DerivedChunkedList::pushChunk;

      using DerivedChunkedList::GenericChunkIterator;

      using DerivedChunkedList::GenericIterator;

      using DerivedChunkedList::GenericSlice;


  };

  template<typename ChunkedListT>
  using Accessor = ChunkedListAccessor<typename ChunkedListT::value_type, ChunkedListT::chunk_size,
                                       typename ChunkedListT::allocator_type>;
} // namespace chunked_list
