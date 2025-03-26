#pragma once

#include <cstring>
#include <functional>
#include <initializer_list>
#include <sstream>

#include "internal/ChunkedListUtility.hpp"

namespace chunked_list {
  /**
   * @class ChunkedList
   * @brief A contiguous-like linear data structure, implemented as a linked list of fixed-size chunks.
   *
   * The chunked list class is designed for efficient storage and access to elements, using chunks of segmented memory to
   * store data.
   *
   * @tparam T The type of elements to be stored in the chunked list
   * @tparam ChunkSize The number of elements in each chunk, with a default value of 32
   */
  template<typename T, size_t ChunkSize = 32>
  class ChunkedList {
      static_assert(ChunkSize > 0, "Chunk Size must be greater than 0");

    protected:
      size_t chunkCount{1};

      class BoundaryError final : std::exception {
          const std::string message = "Out of bounds";

        public:
          BoundaryError() = default;

          explicit BoundaryError(const char *message);

          explicit BoundaryError(const std::string &message);

          explicit BoundaryError(std::string &&message);

          const char *what() const noexcept override;
      };

      template<typename ChunkT, typename ValueT>
      class GenericIterator;

      class Chunk {
          T data[ChunkSize]{};

          size_t nextIndex{0};

          using Iterator = GenericIterator<Chunk, T>;
          using ConstIterator = GenericIterator<const Chunk, const T>;

        public:
          explicit Chunk(Chunk *prevChunk, Chunk *nextChunk = nullptr);

          Chunk(const T *array, size_t size, Chunk *prevChunk = nullptr, Chunk *nextChunk = nullptr);

          explicit Chunk(T value, Chunk *prevChunk = nullptr, Chunk *nextChunk = nullptr);

          Chunk() = default;

          ~Chunk() = default;

          /**
           * @brief Adds the value type publicly to the chunk for type deduction
           */
          using value_type = T;

          /**
           * @brief Adds the chunk size publicly to the chunk as a static member
           */
          static constexpr size_t chunk_size = ChunkSize;

          void push(T value);

          template<typename... Args>
            requires utility::can_construct<T, Args...>
          void emplace(Args &&...args);

          void pop();

          /**
           * @brief returns the chunk offset chunks ahead of the given chunk without accounting for overflows
           */
          Chunk &operator+(size_t offset);

          /**
           * @brief returns the chunk offset chunks behind the given chunk without accounting for overflows
           */
          Chunk &operator-(size_t offset);

          Chunk *nextChunk{nullptr};
          Chunk *prevChunk{nullptr};

          size_t size() const;

          bool empty() const;

          T &operator[](size_t index);

          const T &operator[](size_t index) const;

          T &at(size_t index);

          const T &at(size_t index) const;

          bool operator==(const Chunk &other) const;

          bool operator!=(const Chunk &other) const;

          Iterator begin();

          ConstIterator begin() const;

          Iterator end();

          ConstIterator end() const;

          void getData(std::string &str) const;
      };

      /**
       * @brief The first chunk in the chunked list
       */
      Chunk *front{nullptr};

      /**
       * @brief The most recent chunk in the chunked list
       */
      Chunk *back{nullptr};

      /**
       * @brief simply pushes a chunk to the back, without mutating the chunk count
       */
      template<bool PrevAssigned = false>
      void pushChunk(Chunk *chunk);

      /**
       * @brief A generic chunk iterator for the chunk iterator and const chunk iterator
       * @tparam ChunkT The type of chunk that the iterator will reference (const or non-const)
       */
      template<typename ChunkT>
      class GenericChunkIterator {
          ChunkT *chunk{nullptr};

        public:
          template<typename ChunkIteratorT>
            requires utility::is_chunk_iterator<ChunkedList, ChunkIteratorT>
          explicit GenericChunkIterator(ChunkIteratorT chunkIterator);

          /**
           * @brief The pointer constructor for GenericChunkIterator
           * @param chunkPtr A pointer to a ChunkT object
           */
          explicit GenericChunkIterator(ChunkT *chunkPtr);

          /**
           * @brief The reference constructor for GenericChunkIterator
           * @param chunkRef A reference to a ChunkT object
           */
          explicit GenericChunkIterator(ChunkT &chunkRef);

          ~GenericChunkIterator() = default;

          // stl compatibility

          using value_type = ChunkT;
          using difference_type = std::ptrdiff_t;
          using pointer = ChunkT *;
          using reference = ChunkT &;
          using iterator_category = std::bidirectional_iterator_tag;

          /**
           * @brief Prefix increment operator, incrementing the chunk pointer by one
           * @return The incremented GenericIterator
           */
          GenericChunkIterator operator++();

          /**
           * @brief Postfix increment operator, incrementing the chunk pointer by one
           * @return The original GenericIterator
           */
          GenericChunkIterator operator++(int);

          /**
           * @brief Prefix decrement operator, decrement the chunk pointer by one
           * @return The decremented GenericIterator
           */
          GenericChunkIterator operator--();

          /**
           * @brief Postfix decrement operator, decrementing the chunk pointer by one
           * @return The original GenericIterator
           */
          GenericChunkIterator operator--(int);

          /**
           * @param offset The number of positions to advance the iterator forwards by
           * @return The iterator advanced forward by the given number of positions
           */
          GenericChunkIterator operator+(size_t offset) const;

          /**
           * @param offset The number of positions to move the iterator backwards by
           * @return The iterator moved backwards by the given number of positions
           */
          GenericChunkIterator operator-(size_t offset) const;

          /**
           * @brief Advances the given iterator forwards by a given number of positions
           * @param offset The number of positions to advance the iterator by
           * @return The iterator advanced forward by the given number of positions
           */
          GenericChunkIterator operator+=(size_t offset);

          /**
           * @brief Moves the given iterator backwards by a given number of positions
           * @param offset The number of positions to move the iterator by
           * @return The iterator moved backward by the given number of positions
           */
          GenericChunkIterator operator-=(size_t offset);

          /**
           * @brief Compares the given object with another for equality
           * @tparam ChunkIteratorT The type of object which will be compared to the given iterator
           * @param other The object which will be compared to the given iterator
           * @return Whether the chunk pointer of the given object is equal to the chunk pointer of the other
           */
          template<typename ChunkIteratorT>
            requires utility::is_chunk_iterator<ChunkedList, ChunkIteratorT>
          bool operator==(ChunkIteratorT other) const;

          /**
           * @brief Compares the given object with another for inequality
           * @tparam ChunkIteratorT The type of object which will be compared against the given iterator
           * @param other The object which will be compared against the given iterator
           * @return Whether the chunk pointer of the given object is unequal to the chunk pointer of the other
           */
          template<typename ChunkIteratorT>
            requires utility::is_chunk_iterator<ChunkedList, ChunkIteratorT>
          bool operator!=(ChunkIteratorT other) const;

          /**
           * @brief Dereferences the iterator
           * @return A reference to the chunk, of type ChunkT, stored by the iterator
           */
          ChunkT &operator*();

          /**
           * @brief Const dereferences the iterator
           * @return A const reference to the chunk, of type ChunkT, stored by the iterator
           */
          const ChunkT &operator*() const;

          /**
           * @brief Allows direct access to the members of the chunk stored within the iterator, of type ChunkT
           * @return A pointer to the chunk stored within the iterator, of type ChunkT
           */
          ChunkT *operator->();

          /**
           * @brief Allows direct const access to the members of the chunk stored within the iterator, of type ChunkT
           * @return A const pointer to the chunk stored within the iterator, of type ChunkT
           */
          const ChunkT *operator->() const;
      };

      /**
       * @brief A generic iterator for the iterator and const iterator
       * @tparam ChunkT The type of chunk that the iterator will reference (const or non-const)
       * @tparam ValueT The type of value that the iterator will reference (const or non-const)
       */
      template<typename ChunkT, typename ValueT>
      class GenericIterator {
          using ChunkIteratorT = GenericChunkIterator<ChunkT>;
          using ConstChunkIteratorT = GenericChunkIterator<const ChunkT>;

          ChunkIteratorT chunkIterator{};
          size_t index{0};

        public:
          template<typename IteratorT>
            requires utility::is_iterator<ChunkedList, IteratorT>
          explicit GenericIterator(IteratorT iterator);

          /**
           * @brief Initializes the iterator with a specified chunk pointer and optional index
           */
          explicit GenericIterator(ChunkT *chunkPtr, size_t index = 0);

          /**
           * @brief Initializes the iterator with a specified chunk reference and optional index
           */
          explicit GenericIterator(ChunkT &chunkRef, size_t index = 0);

          /**
           * @brief Initializes the iterator with a specified chunk iterator and optional index
           */
          explicit GenericIterator(ChunkIteratorT chunkIterator, size_t index = 0);

          ~GenericIterator() = default;

          // stl compatibility
          using value_type = ValueT;
          using difference_type = std::ptrdiff_t;
          using pointer = ValueT *;
          using reference = ValueT &;
          using iterator_category = std::bidirectional_iterator_tag;

          /**
           * @brief Prefix increment operator, incrementing the index by one unless it is equal to the ChunkSize, in
           * which case incrementing the chunkIterator member by one
           * @return The incremented generic iterator
           */
          GenericIterator operator++();

          /**
           * @brief Prefix increment operator, incrementing the index by one unless it is equal to the ChunkSize, in
           * which case incrementing the chunkIterator member by one
           * @return The original generic iterator
           */
          GenericIterator operator++(int);

          /**
           * @brief Prefix decrement operator, decrementing the index by one unless it is equal to 0, in which case
           * decrementing the chunkIterator member by one
           * @return The decremented generic iterator
           */
          GenericIterator operator--();

          /**
           * @brief Postfix decrement operator, decrementing the index by one unless it is equal to 0, in which case
           * decrementing the chunkIterator member by one
           * @return The original generic iterator
           */
          GenericIterator operator--(int);

          /**
           * @brief Returns the given iterator advanced forward a given number of positions
           * @param offset The number of positions to advance the iterator by
           * @return The iterator advanced forward by a given number of positions
           */
          GenericIterator operator+(size_t offset);

          /**
           * @brief Returns the given iterator moved backwards a given number of positions
           * @param offset The number of positions to move the iterator by
           * @return The iterator moved backward by a given number of positions
           */
          GenericIterator operator-(size_t offset);

          /**
           * @brief Advances the given iterator forwards by a given number of positions
           * @param offset The number of positions to advance the iterator by
           * @return The iterator advanced forward by the given number of positions
           */
          GenericIterator operator+=(size_t offset);

          /**
           * @brief Moves the given iterator backwards by a given number of positions
           * @param offset The number of positions to move the iterator by
           * @return The iterator moved backward by the given number of positions
           */
          GenericIterator operator-=(size_t offset);

          /**
           * @brief Compares the given object with another for equality
           * @tparam IteratorT The type of object which will be compared to the given iterator
           * @param other The object which will be compared to the given iterator
           * @return Whether the index and chunk iterator of the given object are equal to the index and chunk iterator
           * of the other
           */
          template<typename IteratorT>
            requires utility::is_iterator<ChunkedList, IteratorT>
          bool operator==(IteratorT other) const;

          /**
           * @brief Compares the given object with another for inequality
           * @tparam IteratorT The type of object which will be compared against the given iterator
           * @param other The object which will be compared against the given iterator
           * @return Whether either the index or chunk iterator of the given object is unequal to the index or chunk
           * iterator of the other
           */
          template<typename IteratorT>
            requires utility::is_iterator<ChunkedList, IteratorT>
          bool operator!=(IteratorT other) const;

          /**
           * @brief Dereferences the iterator
           * @return A reference to value, of type ValueT, stored within the current chunk at the current index
           */
          ValueT &operator*();

          /**
           * @brief Dereferences the iterator (const)
           * @return A const reference to the value, of type ValueT, stored within the current chunk at the current
           * index
           */
          const ValueT &operator*() const;

          /**
           * @brief Allows direct access to the members of the object referenced by the iterator, of type ValueT
           * @return A pointer to the object referenced by the iterator, of type ValueT
           */
          ValueT *operator->();

          /**
           * @brief Allows direct const access to the members of the object referenced by the iterator, of type ValueT
           * @return A const pointer to the object referenced by the iterator, of type ValueT
           */
          const ValueT *operator->() const;

          /**
           * @brief A get function, returning the private index member
           * @return The index stored by the given iterator
           */
          size_t getIndex() const;

          /**
           * @brief A get function, returning a reference to the stored chunk
           * @return The chunk stored by the chunk iterator of the given iterator
           */
          ChunkT &getChunk();

          /**
           * @brief A get function, returning a constant reference to the stored chunk
           * @return The chunk stored by the chunk iterator of the given iterator (const)
           */
          const ChunkT &getChunk() const;

          /**
           * @brief A get function, returning a copy of the private chunk iterator member
           * @return The chunkIterator stored by the given iterator
           */
          ChunkIteratorT getChunkIterator();

          /**
           * @brief A get function, returning a constant copy of the private chunk iterator member
           * @return The chunkIterator stored by the given iterator (const)
           */
          ConstChunkIteratorT getChunkIterator() const;
      };

      /**
       * @brief A generic slice for referencing parts of the chunked list
       * @tparam ChunkT The type of chunk that the iterator will reference (const or non-const)
       * @tparam ValueT The type of value that the iterator will reference (const or non-const)
       */
      template<typename ChunkT, typename ValueT>
      class GenericSlice {
          using ChunkIteratorType = GenericChunkIterator<ChunkT>;
          using IteratorType = GenericIterator<ChunkT, ValueT>;
          using ConstIteratorType = GenericIterator<const ChunkT, const ValueT>;

          IteratorType startIterator;
          IteratorType endIterator;

        public:
          /**
           * @brief Constructs a slice of a chunked list from the start iterator to the last iterator
           * (inclusive-exclusive)
           * @param start The iterator for the start of the slice (inclusive)
           * @param end The iterator for the last of the slice (exclusive)
           */
          template<typename StartIteratorT, typename EndIteratorT>
            requires utility::are_iterators<ChunkedList, StartIteratorT, EndIteratorT>
          GenericSlice(StartIteratorT start, EndIteratorT end);

          template<typename StartChunkIteratorT, typename EndChunkIteratorT>
            requires utility::are_chunk_iterators<ChunkedList, StartChunkIteratorT, EndChunkIteratorT>
          GenericSlice(StartChunkIteratorT start, EndChunkIteratorT last);

          ~GenericSlice() = default;

          ValueT &operator[](size_t index);

          const ValueT &operator[](size_t index) const;

          /**
           * @brief Compares the given object with another for equality
           * @tparam SliceT The type of object which will be compared to the given slice
           * @param other The object which will be compared to the given slice
           * @return Whether the iterator of the given object is equal to the iterator of the other object
           */
          template<typename SliceT>
            requires utility::is_slice<ChunkedList, SliceT>
          bool operator==(SliceT other) const;

          /**
           * @brief Compares the given object with another for inequality
           * @tparam SliceT The type of object which will be compared against the given slice
           * @param other The object which will be compared against the given slice
           * @return Whether the iterator of the given object is equal to the iterator of the other object
           */
          template<typename SliceT>
            requires utility::is_slice<ChunkedList, SliceT>
          bool operator!=(SliceT other) const;

          IteratorType begin();

          ConstIteratorType begin() const;

          IteratorType end();

          ConstIteratorType end() const;

          // void expandLeft(size_t distance);

          // void expandRight(size_t distance);
      };

    public:
      /**
       * @brief The default constructor for the chunked list, allocating a single chunk for the front and back
       */
      ChunkedList();

      /**
       * @brief A convenience constructor for the chunked list, allowing items to be added on initialization
       * @param initializerList The initializer list used to determine how the initial chunks will be populated
       */
      ChunkedList(std::initializer_list<T> initializerList);

      /**
       * @brief The destructor for the chunked list, deallocating each chunk starting from the back
       */
      ~ChunkedList();

      /**
       * @brief Adds the value type publicly to the chunked list for type deduction
       */
      using value_type = T;

      /**
       * @brief Adds the chunk size publicly to the chunked list as a static member
       */
      static constexpr size_t chunk_size = ChunkSize;

      /**
       * @brief The non-const chunk iterator class used to iterate through each chunk in the chunked list
       */
      using ChunkIterator = GenericChunkIterator<Chunk>;

      /**
       * @brief The const chunk iterator class used to iterate through each chunk in the chunked list
       */
      using ConstChunkIterator = GenericChunkIterator<const Chunk>;

      /**
       * @brief The non-const iterator class used to iterate through each value of every chunk in the chunked list
       */
      using Iterator = GenericIterator<Chunk, T>;

      /**
       * @brief The const iterator class used to iterate through each value of every chunk in the chunked list
       */
      using ConstIterator = GenericIterator<const Chunk, const T>;

      /**
       * @brief The mutable variation of the slice class
       */
      using Slice = GenericSlice<Chunk, T>;

      /**
       * @brief The immutable variation of the slice class
       */
      using ConstSlice = GenericSlice<const Chunk, const T>;

      /**
       * @brief Chunked list indexing, of O(n) search complexity
       * @param index The index of the element in the chunked list
       * @return A reference to the value at the index
       */
      T &operator[](size_t index);

      /**
       * @brief Const chunked list indexing, of O(n) search complexity
       * @param index The index of the element in the chunked list
       * @return A const reference to the value at the index
       */
      const T &operator[](size_t index) const;

      /**
       * @brief Chunked list indexing, of O(n) search complexity
       * @param index The index of the element in the chunked list
       * @return A Const reference to the value at the index
       */
      T &at(size_t index);

      /**
       * @brief Const chunked list indexing, of O(n) search complexity
       * @param index The index of the element in the chunked list
       * @return A const reference to the value at the index
       */
      const T &at(size_t index) const;

      /**
       * @brief Returns an iterator to the first element
       * @returns An iterator referencing the first element in the chunked list
       */
      Iterator begin();

      /**
       * @brief Returns a const iterator to the first element
       * @returns A const iterator referencing the first element in the chunked list
       */
      ConstIterator begin() const;

      /**
       * @brief Returns an iterator to the element after the last
       * @returns An Iterator referencing the element after the last in the chunked list
       */
      Iterator end();

      /**
       * @brief Returns a const iterator to the element after the last
       * @returns A const iterator referencing the element after the last in the chunked list
       */
      ConstIterator end() const;

      /**
       * @brief Returns a chunk iterator to the first chunk
       * @returns A const iterator referencing the first chunk in the chunked list
       */
      ChunkIterator beginChunk();

      /**
       * @brief Returns a const chunk iterator to the first chunk
       * @returns A const chunk iterator referencing the first chunk in the chunked list
       */
      ConstChunkIterator beginChunk() const;

      /**
       * @brief Returns a chunk iterator to the first chunk
       * @returns A chunk iterator referencing the first chunk in the chunked list
       */
      ChunkIterator endChunk();

      /**
       * @brief Returns a const chunk iterator to a null chunk
       * @returns A chunk iterator not referencing any chunk
       */
      ConstChunkIterator endChunk() const;

      /**
       * @brief Returns a mutable portion of the chunked list from the start to end
       * @param startIndex The index for the start of the slice (inclusive)
       * @param endIndex The index for the end of the slice (exclusive)
       * @return An in-place slice which references the start index to the end index (inclusive-exclusive)
       */
      Slice slice(size_t startIndex, size_t endIndex);

      /**
       * @brief Returns a const portion of the chunked list from the start to end
       * @param startIndex The index for the start of the slice (inclusive)
       * @param endIndex The index for the last of the slice (exclusive)
       * @return An in-place constant slice which references the start index to the end index (inclusive-exclusive)
       */
      ConstSlice slice(size_t startIndex, size_t endIndex) const;

      /**
       * @brief Returns a portion of the chunked list from the start iterator to the end iterator
       * @param start The iterator for the start of the slice (inclusive)
       * @param end The iterator for the end of the slice (exclusive)
       * @return An in-place slice which references the start iterator to the end iterator (inclusive-exclusive)
       */
      template<typename StartIteratorT, typename EndIteratorT>
        requires utility::are_iterators_or_chunk_iterators<ChunkedList, StartIteratorT, EndIteratorT>
      Slice slice(StartIteratorT start, EndIteratorT end);

      /**
       * @brief Returns a const portion of the chunked list from the start iterator to the end iterator
       * @param start The iterator for the start of the slice (inclusive)
       * @param end The iterator for the end of the slice (exclusive)
       * @return An in-place slice which references the start iterator to the end iterator (inclusive-exclusive)
       */
      template<typename StartIteratorT, typename EndIteratorT>
        requires utility::are_iterators_or_chunk_iterators<ChunkedList, StartIteratorT, EndIteratorT>
      ConstSlice slice(StartIteratorT start, EndIteratorT end) const;

      /**
       * @brief Pushes an element to the front of the chunked list
       * @param value The element which will be pushed to the back of the chunked list
       */
      void push(T value);

      /**
       * @brief Pushes an element to the chunked list, but constructs it within the emplace function
       * @tparam Args The types of the arguments used to construct the object
       * @param args The arguments used to construct the object
       */
      template<typename... Args>
        requires utility::can_construct<T, Args...>
      void emplace(Args &&...args);

      /**
       * @brief Pops the most recent item from the back chunk of the chunked list
       */
      void pop();

      /**
       * @brief Pops the back (most recent) chunk from the chunked list
       */
      void popChunk();

      /**
       * @brief Clears the chunked list
       * @tparam RetainFront Whether the front chunk should be retained (additional deallocation required if false)
       */
      template<bool RetainFront = false>
      void clear();

      /**
       * @brief Sorts the chunked list with a template-decided comparison class and sorting algorithm
       * @tparam Compare The comparison functor used in the sorting algorithm
       * @tparam Sort The sorting algorithm which is to be used
       */
      template<typename Compare = std::less<T>, utility::SortType Sort = utility::QuickSort>
      void sort();

      /**
       * @brief Returns the total number of elements stored in the chunked list
       * @return The number of (chunks - 1) multiplied by the chunk size, plus the back next index of the back chunk
       */
      size_t size() const;

      /**
       * @brief Returns whether the chunked list is empty
       * @return Whether the front chunk's next index is 0
       */
      bool empty() const;

      /**
       * @brief Returns whether the given chunked list is equal to the other
       * @param other The chunked list to compare the given one to for equality
       * @return Whether each chunk has the same elements and the lists are both of the same size
       */
      bool operator==(const ChunkedList &other) const;

      /**
       * @brief Returns whether the given chunked list is unequal to the other, regarding the stored elements
       * @param other The chunked list to compare the given one against for inequality
       * @return Whether at least one element is unequal between both containers or the containers are of different
       * sizes
       */
      bool operator!=(const ChunkedList &other) const;

      /**
       * @brief Inserts the chunked list to an ostream
       * @param os The ostream to insert the elements of the chunked list to
       * @param chunkedList The chunked list object
       * @return The ostream object given by the os argument
       */
      template<typename U, size_t>
        requires utility::can_insert<std::ostream, U>
      friend std::ostream &operator<<(std::ostream &os, ChunkedList &chunkedList);

      /**
       * @brief Concatenates the elements of the chunked list
       * @tparam OutputStream The ostream to use for inserting elements
       * @tparam DelimiterType The type of string used to store the delimiter
       * @param delimiter The divider between each element
       * @return Each element of the container concatenated into 1 string
       */
      template<typename OutputStream = std::ostringstream, typename DelimiterType = std::string>
        requires utility::can_insert<OutputStream, T> && utility::can_insert<OutputStream, DelimiterType> &&
                 utility::can_stringify<OutputStream>
      auto concat(DelimiterType delimiter = ", ") -> utility::DeduceStreamStringType<OutputStream>;
  };
} // namespace chunked_list

/**
 * @brief Returns an Iterator to the first element in the given chunked list
 * @tparam T The type stored in the chunked list
 * @tparam ChunkSize The size of each chunk within the chunked list
 * @param chunkedList A reference to the container object
 * @returns An Iterator referencing the first element in the container
 */
template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator
begin(chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept;

/**
 * @brief Returns a const iterator to the first element of the given chunked list
 * @tparam T The type stored in the chunked list
 * @tparam ChunkSize The size of each chunk within the chunked list
 * @param chunkedList A const reference to the chunked list object
 * @returns A const iterator referencing the first element of the chunked list
 */
template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator
begin(const chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept;

/**
 * @brief Returns a const iterator to the end element of the given chunked list
 * @tparam T The type stored in the chunked list
 * @tparam ChunkSize The size of each chunk within the container
 * @param chunkedList A reference to the chunked list object
 * @returns A const iterator referencing the end element of the chunked list
 */
template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator
end(chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept;

/**
 * @brief Returns an iterator referencing the end element of the given chunked list
 * @tparam T The type stored in the container
 * @tparam ChunkSize The size of each chunk within the chunked list
 * @param chunkedList A const reference to the chunked list object
 * @returns An iterator referencing the end element of the chunked list
 */
template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator
end(const chunked_list::ChunkedList<T, ChunkSize> &chunkedList) noexcept;

/**
 * @brief Returns an iterator to the first element of the given chunk
 * @tparam T The type stored in the chunked list
 * @tparam ChunkSize The size of each chunk within the chunked list
 * @param chunk A reference to the chunk object
 * @return An iterator referencing the first element of the chunk
 */
template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator
begin(typename chunked_list::ChunkedList<T, ChunkSize>::Chunk &chunk) noexcept;

/**
 * @brief Returns a const iterator to the first element of the given chunk
 * @tparam T The type stored in the chunked list
 * @tparam ChunkSize The size of each chunk within the chunked list
 * @param chunk A const reference to the chunk object
 * @return A const iterator referencing the first element of the chunk
 */
template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator
begin(const typename chunked_list::ChunkedList<T, ChunkSize>::Chunk &chunk) noexcept;

/**
 * @brief Returns an iterator to the end element of the given chunk
 * @tparam T The type stored in the chunked list
 * @tparam ChunkSize The size of each chunk within the chunked list
 * @param chunk A reference to the chunk object
 * @return An iterator referencing the end element of the chunk
 */
template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::Iterator
end(typename chunked_list::ChunkedList<T, ChunkSize>::Chunk &chunk) noexcept;

/**
 * @brief Returns a const iterator to the first element of the given chunk
 * @tparam T The type stored in the chunked list
 * @tparam ChunkSize The size of each chunk within the chunked list
 * @param chunk A const reference to the chunk object
 * @return A const iterator referencing the end element of the chunk
 */
template<typename T, size_t ChunkSize>
typename chunked_list::ChunkedList<T, ChunkSize>::ConstIterator
end(const typename chunked_list::ChunkedList<T, ChunkSize>::Chunk &chunk) noexcept;

#undef DEBUG_LOG
#undef DEBUG_LINE
#undef DEBUG_EXECUTE

#include "internal/ChunkedList.tpp"
#include "internal/ChunkedListChunk.tpp"
#include "internal/ChunkedListIterator.tpp"
#include "internal/ChunkedListSlice.tpp"
#include "internal/ChunkedListUtility.tpp"
