#pragma once

#include "ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
  class ChunkedListAccessor : public ChunkedList<T, ChunkSize, Allocator> {
    protected:
      using DerivedChunkedList = ChunkedList<T, ChunkSize, Allocator>;

    public:
      using DerivedChunkedList::chunk_count;

      using DerivedChunkedList::ValueAllocator;

      using DerivedChunkedList::ValueAllocatorTraits;

      using DerivedChunkedList::ChunkAllocator;

      using DerivedChunkedList::ChunkAllocatorTraits;

      using DerivedChunkedList::chunk_allocator;

      using DerivedChunkedList::sentinel;

      using DerivedChunkedList::generic_chunk_iterator;

      using DerivedChunkedList::generic_iterator;
  };

  template<typename ChunkedListT>
  using Accessor = ChunkedListAccessor<typename ChunkedListT::value_type, ChunkedListT::chunk_size,
                                       ChunkedListT::template allocator_type>;
} // namespace chunked_list
