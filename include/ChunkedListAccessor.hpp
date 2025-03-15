#pragma once

#include "ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t ChunkSize>
  class ChunkedListAccessor final : public ChunkedList<T, ChunkSize> {
    using DerivedChunkedList = ChunkedList<T, ChunkSize>;

    public:
      using Chunk = typename DerivedChunkedList::Chunk;

      using DerivedChunkedList::pushChunk;

      size_t getChunkCount();

      Chunk *getFront();

      Chunk *getBack();
  };

  template<typename ChunkedListT>
  using Accessor = std::enable_if_t<
    utility::is_template_of_v<ChunkedList, ChunkedListT>,
    ChunkedListAccessor<typename ChunkedListT::value_type, ChunkedListT::chunk_size>
  >;
}

#include "internal/ChunkedListAccessor.tpp"