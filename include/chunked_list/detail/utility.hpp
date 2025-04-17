#pragma once

#include <cstddef>
#include <type_traits>

namespace chunked_list {
  template<typename T, size_t ChunkSize, template<typename> typename Allocator>
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

    template<typename T, size_t ChunkSize, template<typename> typename Allocator>
    struct is_chunked_list<ChunkedList<T, ChunkSize, Allocator>> : std::true_type {};

    template<typename T>
    concept chunked_list = is_chunked_list<T>::value;

    template<typename ChunkedListType, typename T>
    concept iterator = std::is_same_v<T, typename ChunkedListType::iterator> ||
                       std::is_same_v<T, typename ChunkedListType::const_iterator>;

    template<typename ChunkedListType, typename... Types>
    concept are_iterators = (iterator<ChunkedListType, Types> && ...);

    template<typename ChunkedListType, typename T>
    concept chunk_iterator = is_template_of<ChunkedListType::template generic_chunk_iterator, T>;

    template<typename ChunkedListType, typename... Types>
    concept are_chunk_iterators = (chunk_iterator<ChunkedListType, Types> && ...);

    template<typename ChunkedListType, typename T>
    concept iterator_or_chunk_iterator = iterator<ChunkedListType, T> || chunk_iterator<ChunkedListType, T>;

    template<typename ChunkedListType, typename T>
    concept iterator_or_chunk_iterator_const = std::is_same_v<T, typename ChunkedListType::const_iterator> ||
                                               std::is_same_v<T, typename ChunkedListType::const_chunk_iterator>;

    template<typename ChunkedListType, typename... Types>
    concept all_iterators_or_chunk_iterators =
      are_iterators<ChunkedListType, Types...> || are_chunk_iterators<ChunkedListType, Types...>;

    template<typename ChunkedListType, typename T>
    concept iterator_or_chunk_iterator_const_reverse =
      std::is_same_v<T, typename ChunkedListType::const_reverse_iterator>;

    template<typename ChunkedListType, typename T>
    concept iterator_or_chunk_iterator_reverse = std::is_same_v<T, typename ChunkedListType::reverse_iterator> ||
                                                 std::is_same_v<T, typename ChunkedListType::const_reverse_iterator>;

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

    template<typename T, typename ChunkedListType>
    using rebind_value = ChunkedList<T, ChunkedListType::chunk_size, ChunkedListType::template allocator_type>;

    template<size_t ChunkSize, typename ChunkedListType>
    using rebind_chunk_size =
      ChunkedList<typename ChunkedListType::value_type, ChunkSize, ChunkedListType::template allocator_type>;

    template<template<typename> typename Allocator, typename ChunkedListType>
    using rebind_alloc = ChunkedList<typename ChunkedListType::value_type, ChunkedListType::chunk_size, Allocator>;

    namespace sort_functions {
      template<typename Compare, typename T, size_t ChunkSize, template<typename> typename Allocator>
      void bubble_sort(ChunkedList<T, ChunkSize, Allocator> &chunkedList);

      template<typename Compare, typename T, size_t ChunkSize, template<typename> typename Allocator>
      void selection_sort(ChunkedList<T, ChunkSize, Allocator> &chunkedList);

      template<typename Compare, typename T, size_t ChunkSize, template<typename> typename Allocator>
      void insertion_sort(ChunkedList<T, ChunkSize, Allocator> &chunkedList);

      template<typename Compare, typename T, size_t ChunkSize, template<typename> typename Allocator>
      void quick_sort(typename ChunkedList<T, ChunkSize, Allocator>::iterator start,
                      typename ChunkedList<T, ChunkSize, Allocator>::iterator end);

      template<typename Compare, typename T, size_t ChunkSize, template<typename> typename Allocator>
      void heap_sort(ChunkedList<T, ChunkSize, Allocator> &chunkedList);
    } // namespace sort_functions
  } // namespace utility
} // namespace chunked_list

#include "utility.tpp"
