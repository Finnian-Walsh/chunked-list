#pragma once

#include <cstring>
#include <functional>
#include <initializer_list>
#include <memory>
#include <sstream>

#include "detail/utility.hpp"

namespace chunked_list {
  template<typename, size_t, template<typename> typename>
  class ChunkedListAccessor;

  template<typename, bool>
  class ChunkedListSlice;

  /**
   * @class ChunkedList
   * @brief A contiguous-like linear data structure, implemented as a linked list of fixed-size chunks.
   *
   * The chunked list class is designed for efficient storage and access to elements, using chunks of segmented memory
   * to store data.
   *
   * @tparam T The type of elements to be stored in the chunked list
   * @tparam ChunkSize The number of elements in each chunk, with a default value of 32
   * @tparam Allocator The allocator used for the allocation and deallocation of data
   */
  template<typename T, size_t ChunkSize = 32, template<typename> typename Allocator = std::allocator>
  class ChunkedList {
      static_assert(ChunkSize > 0, "Chunk Size must be greater than 0");

      template<typename, size_t, template<typename> typename>
      friend class ChunkedListAccessor;

      template<typename, bool>
      friend class ChunkedListSlice;

      template<bool>
      class generic_iterator;

      size_t chunk_count;

      using ValueAllocator = Allocator<T>;

      using ValueAllocatorTraits = std::allocator_traits<ValueAllocator>;

    public:
      /**
       * @brief The non-const iterator class used to iterate through each value of every chunk in the chunked list
       */
      using iterator = generic_iterator<true>;

      /**
       * @brief The const iterator class used to iterate through each value of every chunk in the chunked list
       */
      using const_iterator = generic_iterator<false>;

      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      class Chunk {
          ValueAllocator value_allocator;

          size_t nextIndex{0};

        public:
          Chunk *prevChunk{nullptr};
          Chunk *nextChunk{nullptr};

        private:
          alignas(T) std::byte array[ChunkSize][sizeof(T)]{};

        public:
          explicit Chunk(Chunk *prevChunk, Chunk *nextChunk = nullptr);

          Chunk(const T *pointer, size_t size, Chunk *prevChunk = nullptr, Chunk *nextChunk = nullptr);

          explicit Chunk(T &&value, Chunk *prevChunk = nullptr, Chunk *nextChunk = nullptr);

          Chunk() = default;

          ~Chunk();

          /**
           * @brief Adds the value type publicly to the chunk for type deduction
           */
          using value_type = T;

          /**
           * @brief Adds the chunk size publicly to the chunk as a static member
           */
          static constexpr size_t chunk_size = ChunkSize;

          template<typename U>
          using allocator_type = Allocator<U>;

          T *data();

          const T *data() const;

          void push_back(T &&value);

          template<typename... Args>
            requires utility::can_construct<T, Args...>
          void emplace_back(Args &&...args);

          void pop_back();

          void clear();

          /**
           * @return the chunk offset chunks ahead of the given chunk without accounting for overflows
           */
          Chunk &operator+(size_t n);

          /**
           * @return returns the chunk offset chunks behind the given chunk without accounting for overflows
           */
          Chunk &operator-(size_t n);

          size_t size() const;

          bool empty() const;

          T &operator[](size_t index);

          const T &operator[](size_t index) const;

          T &at(size_t index);

          const T &at(size_t index) const;

          bool operator==(const Chunk &other) const;

          bool operator!=(const Chunk &other) const;

          iterator begin();

          const_iterator begin() const;

          const_iterator cbegin() const;

          reverse_iterator rbegin();

          const_reverse_iterator rbegin() const;

          const_reverse_iterator crbegin() const;

          iterator end();

          const_iterator end() const;

          const_iterator cend() const;

          reverse_iterator rend();

          const_reverse_iterator rend() const;

          const_reverse_iterator crend() const;
      };

    private:
      using ChunkAllocator = Allocator<Chunk>;

      using ChunkAllocatorTraits = std::allocator_traits<ChunkAllocator>;

      ChunkAllocator chunk_allocator{};

      /**
       * @brief The sentinel chunk in the chunked list
       */
      alignas(Chunk) std::byte sentinel[sizeof(Chunk)]{};

      constexpr Chunk *get_sentinel();

      constexpr const Chunk *get_sentinel() const;

      /**
       * @brief A generic chunk iterator for the chunk iterator and const chunk iterator
       * @tparam Mutable Whether the chunk referenced by the iterator can be mutated
       */
      template<bool Mutable>
      class generic_chunk_iterator {
          using ChunkT = std::conditional_t<Mutable, Chunk, const Chunk>;

          ChunkT *chunk{nullptr};

        public:
          template<typename ChunkIteratorT>
            requires utility::chunk_iterator<ChunkedList, ChunkIteratorT>
          explicit generic_chunk_iterator(ChunkIteratorT chunkIterator);

          /**
           * @brief The pointer constructor for GenericChunkIterator
           * @param chunkPtr A pointer to a ChunkT object
           */
          explicit generic_chunk_iterator(ChunkT *chunkPtr);

          /**
           * @brief The reference constructor for GenericChunkIterator
           * @param chunkRef A reference to a ChunkT object
           */
          explicit generic_chunk_iterator(ChunkT &chunkRef);

          ~generic_chunk_iterator() = default;

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
          generic_chunk_iterator operator++();

          /**
           * @brief Postfix increment operator, incrementing the chunk pointer by one
           * @return The original GenericIterator
           */
          generic_chunk_iterator operator++(int);

          /**
           * @brief Prefix decrement operator, decrement the chunk pointer by one
           * @return The decremented GenericIterator
           */
          generic_chunk_iterator operator--();

          /**
           * @brief Postfix decrement operator, decrementing the chunk pointer by one
           * @return The original GenericIterator
           */
          generic_chunk_iterator operator--(int);

          /**
           * @param offset The number of positions to advance the iterator forwards by
           * @return The iterator advanced forward by the given number of positions
           */
          generic_chunk_iterator operator+(size_t offset) const;

          /**
           * @param offset The number of positions to move the iterator backwards by
           * @return The iterator moved backwards by the given number of positions
           */
          generic_chunk_iterator operator-(size_t offset) const;

          /**
           * @brief Advances the given iterator forwards by a given number of positions
           * @param offset The number of positions to advance the iterator by
           * @return The iterator advanced forward by the given number of positions
           */
          generic_chunk_iterator operator+=(size_t offset);

          /**
           * @brief Moves the given iterator backwards by a given number of positions
           * @param offset The number of positions to move the iterator by
           * @return The iterator moved backward by the given number of positions
           */
          generic_chunk_iterator operator-=(size_t offset);

          /**
           * @brief Compares the given object with another for equality
           * @tparam ChunkIteratorType The type of object which will be compared to the given iterator
           * @param other The object which will be compared to the given iterator
           * @return Whether the chunk pointer of the given object is equal to the chunk pointer of the other
           */
          template<typename ChunkIteratorType>
            requires utility::chunk_iterator<ChunkedList, ChunkIteratorType>
          bool operator==(const ChunkIteratorType &other) const;

          /**
           * @brief Compares the given object with another for inequality
           * @tparam ChunkIteratorType The type of object which will be compared against the given iterator
           * @param other The object which will be compared against the given iterator
           * @return Whether the chunk pointer of the given object is unequal to the chunk pointer of the other
           */
          template<typename ChunkIteratorType>
            requires utility::chunk_iterator<ChunkedList, ChunkIteratorType>
          bool operator!=(const ChunkIteratorType &other) const;

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

          ChunkT &operator[](size_t n);

          const ChunkT &operator[](size_t n) const;

          /**
           * @brief Allows the iterator to be converted to a chunk pointer easily
           */
          operator ChunkT *();

          /**
           * @brief Allows the iterator to be converted to a const chunk pointer easily
           */
          operator const ChunkT *() const;
      };

      /**
       * @brief A generic iterator for the iterator and const iterator
       * @tparam Mutable Whether the value referenced by the iterator can be mutated
       */
      template<bool Mutable>
      class generic_iterator {
          using ChunkT = std::conditional_t<Mutable, Chunk, const Chunk>;
          using ValueT = std::conditional_t<Mutable, T, const T>;

          using chunk_iterator_type = generic_chunk_iterator<Mutable>;
          using const_chunk_iterator_type = generic_chunk_iterator<false>;

          chunk_iterator_type chunkIterator{};
          size_t index{0};

        public:
          template<utility::iterator<ChunkedList> IteratorType>
          explicit generic_iterator(IteratorType iterator);

          /**
           * @brief Initializes the iterator with a specified chunk pointer and optional index
           */
          explicit generic_iterator(ChunkT *chunkPtr, size_t index = 0);

          /**
           * @brief Initializes the iterator with a specified chunk reference and optional index
           */
          explicit generic_iterator(ChunkT &chunkRef, size_t index = 0);

          /**
           * @brief Initializes the iterator with a specified chunk iterator and optional index
           */
          explicit generic_iterator(chunk_iterator_type chunkIterator, size_t index = 0);

          ~generic_iterator() = default;

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
          generic_iterator operator++();

          /**
           * @brief Prefix increment operator, incrementing the index by one unless it is equal to the ChunkSize, in
           * which case incrementing the chunkIterator member by one
           * @return The original generic iterator
           */
          generic_iterator operator++(int);

          /**
           * @brief Prefix decrement operator, decrementing the index by one unless it is equal to 0, in which case
           * decrementing the chunkIterator member by one
           * @return The decremented generic iterator
           */
          generic_iterator operator--();

          /**
           * @brief Postfix decrement operator, decrementing the index by one unless it is equal to 0, in which case
           * decrementing the chunkIterator member by one
           * @return The original generic iterator
           */
          generic_iterator operator--(int);

          /**
           * @param n The number of positions to advance the iterator by
           * @return The iterator advanced forward by a given number of positions
           */
          generic_iterator operator+(size_t n);

          /**
           * @param n The number of positions to move the iterator by
           * @return The iterator moved backward by a given number of positions
           */
          generic_iterator operator-(size_t n);

          /**
           * @param n The number of positions to advance the iterator by
           * @return The iterator advanced forward by the given number of positions
           */
          generic_iterator operator+=(size_t n);

          /**
           * @param n The number of positions to move the iterator by
           * @return The iterator moved backward by the given number of positions
           */
          generic_iterator operator-=(size_t n);

          /**
           * @tparam IteratorType The type of the other object
           * @param other The object which will be compared
           * @return Whether the values are equal
           */
          template<typename IteratorType>
            requires utility::iterator<ChunkedList, IteratorType>
          bool operator==(const IteratorType &other) const;

          /**
           * @tparam IteratorType The type of the other object
           * @param other The object which will be compared
           * @return Whether the values are unequal
           */
          template<typename IteratorType>
            requires utility::iterator<ChunkedList, IteratorType>
          bool operator!=(const IteratorType &other) const;

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

          ValueT &operator[](size_t n);

          const ValueT &operator[](size_t n) const;

          /**
           * @brief A get function, returning the private index member
           * @return The index stored by the given iterator
           */
          size_t get_index() const;

          /**
           * @brief A get function, returning a reference to the stored chunk
           * @return The chunk stored by the chunk iterator of the given iterator
           */
          ChunkT &get_chunk();

          /**
           * @brief A get function, returning a constant reference to the stored chunk
           * @return The chunk stored by the chunk iterator of the given iterator (const)
           */
          const ChunkT &get_chunk() const;

          /**
           * @brief A get function, returning a copy of the private chunk iterator member
           * @return The chunkIterator stored by the given iterator
           */
          chunk_iterator_type get_chunk_iterator();

          /**
           * @brief A get function, returning a constant version of the private chunk iterator member
           * @return The chunkIterator stored by the given iterator (const)
           */
          const_chunk_iterator_type cget_chunk_iterator() const;
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
       * @brief An exceptional class; instances of this are thrown when an index exceeds the boundaries when an at
       * function is called
       */
      class BoundaryError final : std::exception {
          const std::string message = "Out of bounds";

        public:
          BoundaryError() = default;

          explicit BoundaryError(const char *message);

          explicit BoundaryError(const std::string &message);

          explicit BoundaryError(std::string &&message);

          const char *what() const noexcept override;
      };

      /**
       * @brief Adds the value type publicly to the chunked list for type deduction
       */
      using value_type = T;

      /**
       * @brief Adds the chunk size publicly to the chunked list as a static member
       */
      static constexpr size_t chunk_size = ChunkSize;

      /**
       * @brief Adds the allocator type publicly to the chunked list
       */
      template<typename U>
      using allocator_type = Allocator<U>;

      /**
       * @brief The non-const chunk iterator class used to iterate through each chunk in the chunked list
       */
      using chunk_iterator = generic_chunk_iterator<true>;

      /**
       * @brief The const chunk iterator class used to iterate through each chunk in the chunked list
       */
      using const_chunk_iterator = generic_chunk_iterator<false>;

      using reverse_chunk_iterator = std::reverse_iterator<chunk_iterator>;

      using const_reverse_chunk_iterator = std::reverse_iterator<const_chunk_iterator>;

      using mutable_slice = ChunkedListSlice<ChunkedList, true>;

      using immutable_slice = ChunkedListSlice<ChunkedList, false>;

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
       * @return An iterator referencing the first element in the chunked list
       */
      template<typename IteratorType = iterator>
        requires utility::iterator_or_chunk_iterator<ChunkedList, IteratorType>
      IteratorType begin();

      template<typename IteratorType = const_iterator>
        requires utility::iterator_or_chunk_iterator_const<ChunkedList, IteratorType>
      IteratorType begin() const;

      /**
       * @return A const iterator referencing the first element in the chunked list
       */
      template<typename IteratorType = const_iterator>
        requires utility::iterator_or_chunk_iterator_const<ChunkedList, IteratorType>
      IteratorType cbegin() const;

      template<typename ReverseIteratorType = reverse_iterator>
        requires utility::iterator_or_chunk_iterator_reverse<ChunkedList, ReverseIteratorType>
      ReverseIteratorType rbegin();

      template<typename ReverseIteratorType = const_reverse_iterator>
        requires utility::iterator_or_chunk_iterator_const_reverse<ChunkedList, ReverseIteratorType>
      ReverseIteratorType rbegin() const;

      template<typename ReverseIteratorType = const_reverse_iterator>
        requires utility::iterator_or_chunk_iterator_const_reverse<ChunkedList, ReverseIteratorType>
      ReverseIteratorType crbegin() const;

      /**
       * @returns An iterator referencing the element after the last in the chunked list
       */
      template<typename IteratorType = iterator>
        requires utility::iterator_or_chunk_iterator<ChunkedList, IteratorType>
      IteratorType end();

      template<typename IteratorType = const_iterator>
        requires utility::iterator_or_chunk_iterator_const<ChunkedList, IteratorType>
      IteratorType end() const;

      /**
       * @returns A const iterator referencing the element after the last in the chunked list
       */
      template<typename IteratorType = const_iterator>
        requires utility::iterator_or_chunk_iterator_const<ChunkedList, IteratorType>
      IteratorType cend() const;

      /**
       * @returns An iterator referencing the element after the last in the chunked list
       */
      template<typename ReverseIteratorType = reverse_iterator>
        requires utility::iterator_or_chunk_iterator_reverse<ChunkedList, ReverseIteratorType>
      ReverseIteratorType rend();

      template<typename ReverseIteratorType = const_reverse_iterator>
        requires utility::iterator_or_chunk_iterator_const_reverse<ChunkedList, ReverseIteratorType>
      ReverseIteratorType rend() const;

      /**
       * @returns A const iterator referencing the element after the last in the chunked list
       */
      template<typename ReverseIteratorType = const_reverse_iterator>
        requires utility::iterator_or_chunk_iterator_const_reverse<ChunkedList, ReverseIteratorType>
      ReverseIteratorType crend() const;

      /**
       * @param startIndex The index for the start of the slice (inclusive)
       * @param endIndex The index for the end of the slice (exclusive)
       * @return An in-place slice which references the start index to the end index (inclusive-exclusive)
       */
      mutable_slice slice(size_t startIndex, size_t endIndex);

      /**
       * @param startIndex The index for the start of the slice (inclusive)
       * @param endIndex The index for the last of the slice (exclusive)
       * @return An in-place constant slice which references the start index to the end index (inclusive-exclusive)
       */
      immutable_slice slice(size_t startIndex, size_t endIndex) const;

      immutable_slice cslice(size_t startIndex, size_t endIndex) const;

      /**
       * @param start The iterator for the start of the slice (inclusive)
       * @param end The iterator for the end of the slice (exclusive)
       * @return An in-place slice which references the start iterator to the end iterator (inclusive-exclusive)
       */
      template<typename StartIteratorT, typename EndIteratorT>
        requires utility::all_iterators_or_chunk_iterators<ChunkedList, StartIteratorT, EndIteratorT>
      mutable_slice slice(StartIteratorT start, EndIteratorT end);

      /**
       * @param start The iterator for the start of the slice (inclusive)
       * @param end The iterator for the end of the slice (exclusive)
       * @return An in-place slice which references the start iterator to the end iterator (inclusive-exclusive)
       */
      template<typename StartIteratorT, typename EndIteratorT>
        requires utility::all_iterators_or_chunk_iterators<ChunkedList, StartIteratorT, EndIteratorT>
      immutable_slice slice(StartIteratorT start, EndIteratorT end) const;

      template<typename StartIteratorT, typename EndIteratorT>
        requires utility::all_iterators_or_chunk_iterators<ChunkedList, StartIteratorT, EndIteratorT>
      immutable_slice cslice(StartIteratorT start, EndIteratorT end) const;

      /**
       * @brief Pushes an element to the back of the chunked list
       * @param value The element which will be pushed to the back of the chunked list
       */
      void push_back(T &&value);

      void push_back(const T &value);

      /**
       * @brief Pushes an element to the chunked list, but constructs it within the emplace function
       * @tparam Args The types of the arguments used to construct the object
       * @param args The arguments used to construct the object
       */
      template<typename... Args>
        requires utility::can_construct<T, Args...>
      void emplace_back(Args &&...args);

      /**
       * @brief Pops the most recent item from the back chunk of the chunked list
       */
      void pop_back();

      /**
       * @brief Erases an item from the chunked list using an iterator
       * @param it The iterator to be erased
       * @return An iterator referencing the item after the one erased
       */
      iterator erase(iterator it);

      /**
       * @brief Erases a chunk from the chunked list using a chunk iterator
       * @param iterator the chunk iterator to be erased
       * @return A chunk iterator referencing the chunk after the one erased
       */
      chunk_iterator erase(chunk_iterator iterator);

      /**
       * @brief Pops the back (most recent) chunk from the chunked list
       */
      void pop_chunk();

      /**
       * @brief Clears the chunked list
       */
      void clear();

      /**
       * @brief Sorts the chunked list with a template-decided comparison class and sorting algorithm
       * @tparam Compare The comparison functor used in the sorting algorithm
       * @tparam Sort The sorting algorithm which is to be used
       */
      template<typename Compare = std::less<T>, utility::SortType Sort = utility::QuickSort>
      void sort();

      /**
       * @return The number of (chunks - 1) multiplied by the chunk size, plus the back next index of the back chunk
       */
      size_t size() const;

      /**
       * @return Whether the front chunk's next index is 0
       */
      bool empty() const;

      /**
       * @param other The chunked list to compare the given one to for equality
       * @return Whether each chunk has the same elements and the lists are both of the same size
       */
      bool operator==(const ChunkedList &other) const;

      /**
       * @param other The chunked list to compare the given one against for inequality
       * @return Whether at least one element is unequal between both lists or the lists are of different sizes
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
 * @brief Standard global begin function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return An iterator referencing the first element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename IteratorType = typename ChunkedListType::iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator<ChunkedListType, IteratorType>
IteratorType begin(ChunkedListType &chunkedList) noexcept;

/**
 * @brief Const begin function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return A const iterator referencing the first element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ConstIteratorType = typename ChunkedListType::const_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_const<ChunkedListType, ConstIteratorType>
ConstIteratorType begin(const ChunkedListType &chunkedList) noexcept;

/**
 * @brief Const begin function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return A const iterator referencing the first element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ConstIteratorType = typename ChunkedListType::const_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_const<ChunkedListType, ConstIteratorType>
ConstIteratorType cbegin(const ChunkedListType &chunkedList) noexcept;

/**
 * @brief Reverse begin function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A reference to the chunked list object
 * @return A reverse iterator referencing the last element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ReverseIteratorType = typename ChunkedListType::reverse_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_reverse<ChunkedListType, ReverseIteratorType>
ReverseIteratorType rbegin(ChunkedListType &chunkedList) noexcept;

/**
 * @brief Const reverse begin function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return A reverse const iterator referencing the last element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ConstReverseIteratorType = typename ChunkedListType::const_reverse_iterator>
ConstReverseIteratorType rbegin(const ChunkedListType &chunkedList) noexcept;

/**
 * @brief Explicit const reverse begin function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return A reverse const iterator referencing the last element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ConstReverseIteratorType = typename ChunkedListType::const_reverse_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_const_reverse<ChunkedListType, ConstReverseIteratorType>
ConstReverseIteratorType crbegin(const ChunkedListType &chunkedList) noexcept;

/**
 * @brief Standard end function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A reference to the chunked list object
 * @return An iterator referencing the end element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename IteratorType = typename ChunkedListType::iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator<ChunkedListType, IteratorType>
IteratorType end(ChunkedListType &chunkedList) noexcept;

/**
 * @brief Const end function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return A const iterator referencing the end element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ConstIteratorType = typename ChunkedListType::const_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_const<ChunkedListType, ConstIteratorType>
ConstIteratorType end(const ChunkedListType &chunkedList) noexcept;

/**
 * @brief Explicit const end function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return A const iterator referencing the end element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ConstIteratorType = typename ChunkedListType::const_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_const<ChunkedListType, ConstIteratorType>
ConstIteratorType cend(const ChunkedListType &chunkedList) noexcept;

/**
 * @brief Reverse end function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A reference to the chunked list object
 * @return A reverse iterator referencing the first element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ReverseIteratorType = typename ChunkedListType::reverse_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_reverse<ChunkedListType, ReverseIteratorType>
ReverseIteratorType rend(ChunkedListType &chunkedList) noexcept;

/**
 * @brief Const reverse end function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return A const reverse iterator referencing the first element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ConstReverseIteratorType = typename ChunkedListType::const_reverse_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_const_reverse<ChunkedListType, ConstReverseIteratorType>
ConstReverseIteratorType rend(const ChunkedListType &chunkedList) noexcept;

/**
 * @brief Explicit const reverse end function for chunked list
 * @tparam ChunkedListType The type of chunked list
 * @param chunkedList A const reference to the chunked list object
 * @return A const reverse iterator referencing the first element / chunk of the chunked list
 */
template<chunked_list::utility::chunked_list ChunkedListType,
         typename ConstReverseIteratorType = typename ChunkedListType::const_reverse_iterator>
  requires chunked_list::utility::iterator_or_chunk_iterator_const_reverse<ChunkedListType, ConstReverseIteratorType>
ConstReverseIteratorType crend(const ChunkedListType &chunkedList) noexcept;

#include "detail/Chunk.tpp"
#include "detail/ChunkedList.tpp"
#include "detail/ChunkedListSlice.hpp"
#include "detail/ChunkedListSlice.tpp"
#include "detail/generic_iterator.tpp"
#include "detail/utility.tpp"
