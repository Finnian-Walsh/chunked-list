#pragma once

#include <sstream>
#include <initializer_list>
#include <cstring>
#include <functional>

#include "internal/ChunkedListUtility.hpp"

/**
 * @class ChunkedList
 * @brief A contiguous-like linear data structure, implemented as a linked list of fixed-size Chunks.
 *
 * The ChunkedList class is designed for efficient storage and access to elements, using Chunks of segmented memory to store data.
 *
 * @tparam T The type of elements to be stored in the ChunkedList
 * @tparam ChunkSize The number of elements in each Chunk, with a default value of 32
 */
template<typename T, size_t ChunkSize = 32>
class ChunkedList {
  protected:
    static_assert(ChunkSize > 0, "Chunk Size must be greater than 0");

    template<typename OutputStream>
    using DeduceStringType = decltype(std::declval<OutputStream>().str());

    size_t chunkCount{1};

    class Chunk {
      T data[ChunkSize]{};

      public:
        Chunk(Chunk *nextChunk, Chunk *prevChunk);

        Chunk(const T *array, size_t size, Chunk *nextChunk = nullptr, Chunk *prevChunk = nullptr);

        explicit Chunk(T value);

        Chunk() = default;

        ~Chunk() = default;

        /**
         * @brief returns the chunk x chunks ahead of the given chunk. Does not account for overflows
         */
        Chunk &operator+(size_t offset);

        /**
         * @brief returns the chunk x chunks behind the given chunk. Does not account for overflows
         */
        Chunk &operator-(size_t offset);

        size_t nextIndex{0};

        Chunk *nextChunk{nullptr};
        Chunk *prevChunk{nullptr};

        bool empty() const;

        T &operator[](size_t index);

        const T &operator[](size_t index) const;

        bool operator==(const Chunk &other) const;

        bool operator!=(const Chunk &other) const;

        void debugData(std::string &str) const;
    };

    /**
     * @brief The first Chunk in the ChunkedList
     */
    Chunk *front{nullptr};

    /**
     * @brief The most recent Chunk in the ChunkedList
     */
    Chunk *back{nullptr};

    /**
     * @brief simply pushes a chunk to the back, without mutating the chunkCount
     */
    void pushChunk(Chunk *chunk);

    template<typename ChunkT>
    class GenericChunkIterator {
      public:
        // stl compatibility

        using value_type = ChunkT;
        using difference_type = std::ptrdiff_t;
        using pointer = ChunkT *;
        using reference = ChunkT &;
        using iterator_category = std::bidirectional_iterator_tag;

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
         * @return True if the given object's chunk pointer is equal to the other's, otherwise false
         */
        template<typename ChunkIteratorT>
        bool operator==(ChunkIteratorT other) const;

        /**
         * @brief Compares the given object with another for inequality
         * @tparam ChunkIteratorT The type of object which will be compared to the given iterator
         * @param other The object which will be compared to the given iterator
         * @return True if the given object's chunk is unequal to the other's, otherwise false
         */
        template<typename ChunkIteratorT>
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

      private:
        ChunkT *chunk{nullptr};
    };

    template<typename ChunkT, typename ValueT>
    class GenericIterator {
      using ChunkIteratorT = GenericChunkIterator<ChunkT>;

      public:
        // stl compatibility
        using value_type = ValueT;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueT *;
        using reference = ValueT &;
        using iterator_category = std::bidirectional_iterator_tag;

        /**
         * @brief Initialises the GenericIterator with a specified chunk pointer and optional index
         */
        explicit GenericIterator(ChunkT *chunkPtr, size_t index = 0);

        /**
         * @brief Initialises the GenericIterator with a specified chunk reference and optional index
         */
        explicit GenericIterator(ChunkT &chunkRef, size_t index = 0);

        /**
         * @brief Initialises the GenericIterator with a specified generic ChunkIterator and optional index
         */
        explicit GenericIterator(ChunkIteratorT chunkIterator, size_t index = 0);

        ~GenericIterator() = default;

        /**
         * @brief Prefix increment operator, incrementing the index by one unless it is equal to the ChunkSize, in which case incrementing the chunkIterator member by one
         * @return The incremented GenericIterator
         */
        GenericIterator operator++();

        /**
         * @brief Prefix increment operator, incrementing the index by one unless it is equal to the ChunkSize, in which case incrementing the chunkIterator member by one
         * @return The original GenericIterator
         */
        GenericIterator operator++(int);

        /**
         * @brief Prefix decrement operator, decrementing the index by one unless it is equal to 0, in which case decrementing the chunkIterator member by one
         * @return The decremented GenericIterator
         */
        GenericIterator operator--();

        /**
         * @brief Postfix decrement operator, decrementing the index by one unless it is equal to 0, in which case decrementing the chunkIterator member by one
         * @return The original GenericIterator
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
         * @return True if the given object's index and chunkIterator are equal to the other's index and chunkIterator
         */
        template<typename IteratorT>
        bool operator==(IteratorT other) const;

        /**
         * @brief Compares the given object with another for inequality
         * @tparam IteratorT The type of object which will be compared to the given iterator
         * @param other The object which will be compared to the given iterator
         * @return True if the given object's index is unequal to the other's or the given object's chunkIterator is unequal to the other's, otherwise false
         */
        template<typename IteratorT>
        bool operator!=(IteratorT other) const;

        /**
         * @brief Dereferences the iterator
         * @return A reference to value, of type ValueT, stored within the current chunk at the current index
         */
        ValueT &operator*();

        /**
         * @brief Dereferences the iterator (const)
         * @return A const reference to the value, of type ValueT, stored within the current chunk at the current index
         */
        const ValueT &operator*() const;

        /**
         * @brief Allows direct access to the members of the object "stored" within the iterator, of type ValueT
         * @return A pointer to the object "stored" within the iterator, of type ValueT
         */
        ValueT *operator->();

        /**
         * @brief Allows direct const access to the members of the object "stored" within the iterator, of type ValueT
         * @return A const pointer to the object "stored" within the iterator, of type ValueT
         */
        const ValueT *operator->() const;

        /**
         * @brief A get function, returning the private index member
         * @return The index stored by the given iterator
         */
        size_t getIndex() const;

        /**
         * @brief A get function, returning the private chunkIterator member
         * @return The chunkIterator stored by the given iterator
         */
        ChunkIteratorT getChunkIterator() const;

      private:
        ChunkIteratorT chunkIterator{};
        size_t index{0};
    };

  public:
    /**
     * @brief The default constructor for ChunkedList, allocating a single Chunk for the front and back
     */
    ChunkedList();

    /**
     * @brief A convenience constructor for ChunkedList, allowing items to be added on initialisation
     * @param initializerList The initializer_list used to determine how the initial chunks will be populated
     */
    ChunkedList(std::initializer_list<T> initializerList);

    /**
     * @brief The destructor for ChunkedList, deallocating each chunk starting from the back
     */
    ~ChunkedList();

    /**
    * @brief The non-const ChunkIterator class used to iterate through each Chunk in the ChunkedList
    */
    using ChunkIterator = GenericChunkIterator<Chunk>;

    /**
    * @brief The const ChunkIterator class used to iterate through each Chunk in the ChunkedList
    */
    using ConstChunkIterator = GenericChunkIterator<const Chunk>;

    /**
     * @brief The non-const Iterator class used to iterate through each value of every Chunk in the ChunkedList
     */
    using Iterator = GenericIterator<Chunk, T>;

    /**
     * @brief The const Iterator class used to iterate through each value of every Chunk in the ChunkedList
     */
    using ConstIterator = GenericIterator<const Chunk, const T>;

    /**
     * @brief Direct ChunkedList indexing, of O(n/k) search complexity, where n is the index and k is the ChunkSize
     * @param index The index of the element in the ChunkedList
     * @return A reference to the value at the index
     */
    T &operator[](size_t index);

    /**
      * @brief Direct const ChunkedList indexing, of O(n/k) search complexity, where n is the index and k is the ChunkSize
      * @param index The index of the element in the ChunkedList
      * @return A const reference to the value at the index
      */
    const T &operator[](size_t index) const;

    /**
     * @brief Returns an Iterator to the first element
     * @returns An Iterator referencing the first element in the container
     */
    Iterator begin();

    /**
     * @brief Returns a ConstIterator to the first element
     * @returns A ConstIterator referencing the first element in the container
     */
    ConstIterator begin() const;

    /**
     * @brief Returns an Iterator to the element after the last
     * @returns An Iterator referencing the element after the last in the container
     */
    Iterator end();

    /**
     * @brief Returns a ConstIterator to the element after the last
     * @returns A ConstIterator referencing the element after the last in the container
     */
    ConstIterator end() const;

    /**
     * @brief Returns a ChunkIterator to the first Chunk
     * @returns A ChunkIterator referencing the first Chunk in the container
     */
    ChunkIterator beginChunk();

    /**
     * @brief Returns a ConstChunkIterator to the first Chunk
     * @returns A ConstChunkIterator referencing the first Chunk in the container
     */
    ConstChunkIterator beginChunk() const;

    /**
     * @brief Returns a ChunkIterator to the first Chunk
     * @returns A ChunkIterator referencing the first Chunk in the container
     */
    ChunkIterator endChunk();

    /**
     * @brief Returns a ConstChunkIterator to a null Chunk
     * @returns A ChunkIterator not referencing any Chunk
     */
    ConstChunkIterator endChunk() const;

    /**
     * @brief Pushes an element to the front of the ChunkedList
     * @param value The element which will be pushed to the back of the container
     */
    void push(T value);

    /**
     * @brief Pushes an element to the ChunkedList, but constructs it within the emplace function
     * @tparam Args The types of the arguments used to construct the object
     * @param args The arguments used to construct the object
     */
    template<typename... Args>
    void emplace(Args &&... args);

    /**
     * @brief Pops the most recent item from the back Chunk of the ChunkedList
     */
    void pop();

    /**
     * @brief Pops the back (most recent) Chunk from the ChunkedList
     */
    void popChunk();

    /**
     * @brief Sorts the ChunkedList with a template-decided comparison class and sorting algorithm
     * @tparam Compare The Compare class used in the sorting algorithm
     * @tparam Sort The sorting algorithm which should be used
     */
    template<typename Compare = std::less<T>, SortType Sort = QuickSort>
    void sort();

    /**
     * @brief Returns the total number of elements stored in the ChunkedList
     * @return The number of Chunks - 1 multiplied by the ChunkSize, plus the back Chunk's next index
     */
    size_t size() const;

    /**
     * @brief Returns whether the ChunkedList is empty
     * @return True if front Chunk's next index is 0, otherwise false
     */
    bool empty() const;

    /**
     * @brief Returns whether the given ChunkedList is equal to the other
     * @param other The ChunkedList to compare the given one to, for equality
     * @return True if each Chunk has the same elements, and the ChunkedLists are both of the same size
     */
    bool operator==(const ChunkedList &other) const;

    /**
     * @brief Returns whether the given ChunkedList is unequal to the other, regarding the stored elements
     * @param other The container to compare the given one to, for inequality
     * @return True if at least one element is unequal between both containers
     */
    bool operator!=(const ChunkedList &other) const;

    /**
     * @brief Inserts the ChunkedList to an ostream
     * @param os The ostream to insert the container's elements to
     * @param chunkedList The given container
     * @return The ostream object given by the os argument
     */
    template<typename, size_t, bool>
    friend std::ostream &operator<<(std::ostream &os, ChunkedList &chunkedList);

    /**
     * @brief Concatenates the elements of the ChunkedList
     * @tparam OutputStream The ostream to use for inserting elements
     * @tparam SeparatorType The type of string used to store the delimiter
     * @param delimiter The divider between each element
     * @return Each element of the container concatenated into 1 string
     */
    template<
      typename OutputStream = std::ostringstream,
      typename SeparatorType = std::string>
    auto concat(SeparatorType delimiter = ", ") -> DeduceStringType<OutputStream>;
};

/**
 * @brief Returns an Iterator to the first element in the given ChunkedList
 * @tparam T The type stored in the container
 * @tparam ChunkSize The size of each Chunk within the container
 * @param chunkedList A reference to the container object
 * @returns An Iterator referencing the first element in the container
 */
template<typename T, size_t ChunkSize>
typename ChunkedList<T, ChunkSize>::Iterator
begin(ChunkedList<T, ChunkSize> &chunkedList) noexcept;

/**
 * @brief Returns a ConstIterator to the first element in the given ChunkedList
 * @tparam T The type stored in the container
 * @tparam ChunkSize The size of each Chunk within the container
 * @param chunkedList A const reference to the container object
 * @returns A ConstIterator referencing the first element in the container
 */
template<typename T, size_t ChunkSize>
typename ChunkedList<T, ChunkSize>::ConstIterator
begin(const ChunkedList<T, ChunkSize> &chunkedList) noexcept;

/**
 * @brief Returns a ConstIterator to the element after the last in the given ChunkedList
 * @tparam T The type stored in the container
 * @tparam ChunkSize The size of each Chunk within the container
 * @param chunkedList A reference to the container object
 * @returns A ConstIterator referencing the element after the last in the container
 */
template<typename T, size_t ChunkSize>
typename ChunkedList<T, ChunkSize>::Iterator
end(ChunkedList<T, ChunkSize> &chunkedList) noexcept;

/**
 * @brief Returns an Iterator to the element after the last in the given ChunkedList
 * @tparam T The type stored in the container
 * @tparam ChunkSize The size of each Chunk within the container
 * @param chunkedList A const reference to the container object
 * @returns An Iterator referencing the element after the last in the container
 */
template<typename T, size_t ChunkSize>
typename ChunkedList<T, ChunkSize>::ConstIterator
end(const ChunkedList<T, ChunkSize> &chunkedList) noexcept;

#undef DEBUG_LOG
#undef DEBUG_LINE
#undef DEBUG_EXECUTE

#include "../src/ChunkedList.tpp"
#include "../src/ChunkedListChunk.tpp"
#include "../src/ChunkedListIterator.tpp"
#include "../src/ChunkedListUtility.tpp"
