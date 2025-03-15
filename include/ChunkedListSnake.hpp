#pragma once

#include "ChunkedList.hpp"

namespace chunked_list {
  template<typename T, size_t Chunk_Size = 32>
  class Chunked_List final : ChunkedList<T, Chunk_Size> {
    using derived_chunked_list = ChunkedList<T, Chunk_Size>;

    public:
      using derived_chunked_list::ChunkedList;

      using derived_chunked_list::value_type;

      using derived_chunked_list::chunk_size;

      using chunk_iterator = typename derived_chunked_list::ChunkIterator;

      using const_chunk_iterator = typename derived_chunked_list::ConstChunkIterator;

      using iterator = typename derived_chunked_list::Iterator;

      using const_iterator = typename derived_chunked_list::ConstIterator;

      using slice = typename derived_chunked_list::Slice;

      using const_slice = typename derived_chunked_list::ConstSlice;

      using derived_chunked_list::operator[];

      using derived_chunked_list::begin;

      using derived_chunked_list::end;

      chunk_iterator begin_chunk();

      const_chunk_iterator begin_chunk() const;

      chunk_iterator end_chunk();

      const_chunk_iterator end_chunk() const;

      slice get_slice(size_t start_index, size_t end_index);

      const_slice get_slice(size_t start_index, size_t end_index) const;

      template<typename Begin_Iterator_Type, typename End_Iterator_Type>
        requires utility::are_iterators_or_chunk_iterators<derived_chunked_list, Begin_Iterator_Type, End_Iterator_Type>
      slice get_slice(Begin_Iterator_Type begin, End_Iterator_Type end);

      template<typename Begin_Iterator_Type, typename End_Iterator_Type>
        requires utility::are_iterators_or_chunk_iterators<derived_chunked_list, Begin_Iterator_Type, End_Iterator_Type>
      const_slice get_slice(Begin_Iterator_Type begin, End_Iterator_Type end) const;

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

      bool operator==(const Chunked_List &other) const;

      bool operator!=(const Chunked_List &other) const;

      using derived_chunked_list::concat;

      template<typename, size_t, bool>
      friend std::ostream &operator<<(std::ostream &os, Chunked_List &chunked_list);
  };
}

template<typename T, size_t Chunk_Size>
typename chunked_list::Chunked_List<T, Chunk_Size>::iterator
begin(chunked_list::Chunked_List<T, Chunk_Size> &chunked_list);

template<typename T, size_t Chunk_Size>
typename chunked_list::Chunked_List<T, Chunk_Size>::const_iterator
begin(const chunked_list::Chunked_List<T, Chunk_Size> &chunked_list);

template<typename T, size_t Chunk_Size>
typename chunked_list::Chunked_List<T, Chunk_Size>::iterator
end(chunked_list::Chunked_List<T, Chunk_Size> &chunked_list);

template<typename T, size_t Chunk_Size>
typename chunked_list::Chunked_List<T, Chunk_Size>::const_iterator
end(const chunked_list::Chunked_List<T, Chunk_Size> &chunked_list);

#include "internal/ChunkedListSnake.tpp"
