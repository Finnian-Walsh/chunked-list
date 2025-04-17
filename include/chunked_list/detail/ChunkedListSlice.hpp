#pragma once

#include "../ChunkedList.hpp"

namespace chunked_list {
  /**
   * @brief A generic slice for referencing parts of the chunked list
   * @tparam ChunkedListType The type of chunked list that the slice will reference
   * @tparam Mutable Whether mutating the memory stored is allowed
   */
  template<typename ChunkedListType, bool Mutable>
  class ChunkedListSlice {

      using ValueT =
        std::conditional_t<Mutable, typename ChunkedListType::value_type, const typename ChunkedListType::value_type>;
      using ChunkT =
        std::conditional_t<Mutable, typename ChunkedListType::Chunk, const typename ChunkedListType::Chunk>;

      using chunk_iterator = typename ChunkedListType::template generic_chunk_iterator<Mutable>;
      using iterator_type = typename ChunkedListType::template generic_iterator<Mutable>;
      using const_iterator_type = typename ChunkedListType::const_iterator;

      iterator_type startIterator;
      iterator_type endIterator;

    public:
      /**
       * @brief Constructs a slice of a chunked list from the start iterator to the last iterator
       * (inclusive-exclusive)
       * @param start The iterator for the start of the slice (inclusive)
       * @param end The iterator for the last of the slice (exclusive)
       */
      template<typename StartIteratorT, typename EndIteratorT>
        requires utility::are_iterators<ChunkedListType, StartIteratorT, EndIteratorT>
      ChunkedListSlice(StartIteratorT start, EndIteratorT end);

      template<typename StartChunkIteratorT, typename EndChunkIteratorT>
        requires utility::are_chunk_iterators<ChunkedListType, StartChunkIteratorT, EndChunkIteratorT>
      ChunkedListSlice(StartChunkIteratorT start, EndChunkIteratorT last);

      ~ChunkedListSlice() = default;

      ValueT &operator[](size_t index);

      const ValueT &operator[](size_t index) const;

      /**
       * @brief Compares the given object with another for equality
       * @tparam SliceT The type of object which will be compared to the given slice
       * @param other The object which will be compared to the given slice
       * @return Whether the iterator of the given object is equal to the iterator of the other object
       */
      template<typename SliceT>
      bool operator==(SliceT other) const;

      /**
       * @brief Compares the given object with another for inequality
       * @tparam SliceT The type of object which will be compared against the given slice
       * @param other The object which will be compared against the given slice
       * @return Whether the iterator of the given object is equal to the iterator of the other object
       */
      template<typename SliceT>
      bool operator!=(SliceT other) const;

      iterator_type begin();

      const_iterator_type cbegin() const;

      iterator_type end();

      const_iterator_type cend() const;
  };
} // namespace chunked_list
