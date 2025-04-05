#pragma once

#include <cstddef>
#include <type_traits>

namespace chunked_list {
  template<typename T, size_t ChunkSize, typename Allocator>
  class ChunkedList;

  namespace utility {
    enum SortType {
      BubbleSort,
      SelectionSort,
      InsertionSort,
      QuickSort,
      HeapSort,
    };

    template<typename OutputStream>
    using DeduceStreamStringType = decltype(std::declval<OutputStream>().str());

    template<template<typename...> typename TemplateT, typename T>
    class template_of {
        template<typename>
        class Impl : public std::false_type {};

        template<typename... Args>
        class Impl<TemplateT<Args...>> : public std::true_type {};

      public:
        static constexpr bool value = Impl<T>::value;
    };

    template<template<typename...> typename TemplateT, typename T>
    concept is_template_of = template_of<TemplateT, T>::value;

    template<typename>
    struct is_chunked_list : std::false_type {};

    template<typename T, size_t ChunkSize, typename Allocator>
    struct is_chunked_list<ChunkedList<T, ChunkSize, Allocator>> : std::true_type {};

    template<typename T>
    concept is_chunked_list_v = is_chunked_list<T>::value;

    template<typename ChunkedListT, typename IteratorT>
    concept is_iterator = is_template_of<ChunkedListT::template GenericIterator, IteratorT>;

    template<typename ChunkedListT, typename... IteratorTs>
    concept are_iterators = (is_iterator<ChunkedListT, IteratorTs> && ...);

    template<typename ChunkedListT, typename ChunkIteratorT>
    concept is_chunk_iterator = is_template_of<ChunkedListT::template GenericChunkIterator, ChunkIteratorT>;

    template<typename ChunkedListT, typename... ChunkIteratorTs>
    concept are_chunk_iterators = (is_chunk_iterator<ChunkedListT, ChunkIteratorTs> && ...);

    template<typename ChunkedListT, typename... IteratorTs>
    concept are_iterators_or_chunk_iterators =
      are_iterators<ChunkedListT, IteratorTs...> || are_chunk_iterators<ChunkedListT, IteratorTs...>;

    template<typename ChunkedListT, typename SliceT>
    concept is_slice = is_template_of<ChunkedListT::template GenericSlice, SliceT>;

    template<typename OutputStream, typename T>
    concept can_insert = requires(OutputStream os, T obj) {
      std::is_reference_v<std::decay_t<decltype(os << obj)>>;
      { os << obj };
      std::is_base_of_v<std::remove_reference_t<decltype(os << obj)>, OutputStream>;
    };

    template<typename OutputStream>
    concept can_stringify = requires(OutputStream os) {
      { os.str() };
    };

    template<typename T, typename... Args>
    concept can_construct = requires(T obj, Args... args) {
      { T{args...} };
    };

    template<typename T>
    concept can_to_string = requires(T obj) {
      { std::to_string(obj) } -> std::same_as<std::string>;
    };

    template<typename T>
    concept string_compatible =
      can_construct<std::string, T> || can_to_string<T> || can_stringify<T> || can_insert<std::ostringstream, T>;

    template<string_compatible T>
    std::string intoString(T value);

    template<string_compatible... Args>
    std::string concatenate(Args &&...args);

    template<typename T, typename ChunkedListT>
    using remap_value = ChunkedList<T, ChunkedListT::chunk_size, typename ChunkedListT::allocator_type>;

    template<size_t ChunkSize, typename ChunkedListT>
    using remap_chunk_size =
      ChunkedList<typename ChunkedListT::value_type, ChunkSize, typename ChunkedListT::allocator_type>;

    template<typename Allocator, typename ChunkedListT>
    using remap_alloc = ChunkedList<typename ChunkedListT::value_type, ChunkedListT::chunk_size, Allocator>;

    namespace sort_functions {
      template<typename Compare, typename T, size_t ChunkSize, typename Allocator>
      void bubble_sort(ChunkedList<T, ChunkSize, Allocator> &chunkedList);

      template<typename Compare, typename T, size_t ChunkSize, typename Allocator>
      void selection_sort(ChunkedList<T, ChunkSize, Allocator> &chunkedList);

      template<typename Compare, typename T, size_t ChunkSize, typename Allocator>
      void insertion_sort(ChunkedList<T, ChunkSize, Allocator> &chunkedList);

      template<typename Compare, typename T, size_t ChunkSize, typename Allocator>
      void quick_sort(typename ChunkedList<T, ChunkSize, Allocator>::Iterator start,
                      typename ChunkedList<T, ChunkSize, Allocator>::Iterator end);

      template<typename Compare, typename T, size_t ChunkSize, typename Allocator>
      void heap_sort(ChunkedList<T, ChunkSize, Allocator> &chunkedList);
    } // namespace sort_functions
  } // namespace utility
} // namespace chunked_list

#include "ChunkedListUtility.tpp"
