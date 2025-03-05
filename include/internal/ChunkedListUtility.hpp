#pragma once

#include <type_traits>
#include <cstddef>

template<typename T, size_t ChunkSize>
class ChunkedList;

namespace chunked_list_utility {
  enum SortType {
    BubbleSort,
    SelectionSort,
    InsertionSort,
    QuickSort,
    HeapSort,
  };

  template<template <typename...> typename TemplateT, typename T>
  class is_template_of {
    template<typename>
    class Impl : public std::false_type {
    };

    template<typename... Args>
    class Impl<TemplateT<Args...> > : public std::true_type {
    };

    public:
      static constexpr bool value = Impl<T>::value;
  };

  template<template <typename...> class Template, typename T>
  concept template_of = is_template_of<Template, T>::value;

  template<typename ChunkedListT, typename T>
  concept is_generic_iterator = template_of<ChunkedListT::template GenericIterator, T>;

  template<typename ChunkedListT, typename T>
  concept is_generic_chunk_iterator = template_of<ChunkedListT::template GenericChunkIterator, T>;

  template<typename OutputStream, typename T>
  concept can_insert = requires(OutputStream os, T obj)
  {
    { os << obj };
    std::is_base_of_v<std::remove_reference_t<decltype(os << obj)>, OutputStream>;
  };

  /**
   * @brief Calls the given sort function on the chunked list
   */
  template<typename Compare, SortType Sort, typename T, size_t ChunkSize>
  void sort(ChunkedList<T, ChunkSize> &chunkedList);

  namespace sort_functions {
    template<typename Compare, typename T, size_t ChunkSize>
    void bubble_sort(ChunkedList<T, ChunkSize> &chunkedList);

    template<typename Compare, typename T, size_t ChunkSize>
    void selection_sort(ChunkedList<T, ChunkSize> &chunkedList);

    template<typename Compare, typename T, size_t ChunkSize>
    void insertion_sort(ChunkedList<T, ChunkSize> &chunkedList);

    template<typename Compare, typename T, size_t ChunkSize>
    void quick_sort(typename ChunkedList<T, ChunkSize>::Iterator start,
                    typename ChunkedList<T, ChunkSize>::Iterator end);

    template<typename Compare, typename T, size_t ChunkSize>
    void heap_sort(ChunkedList<T, ChunkSize> &chunkedList);
  }
}

#include "../src/ChunkedListUtility.tpp"
