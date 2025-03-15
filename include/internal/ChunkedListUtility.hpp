#pragma once

#include <type_traits>
#include <cstddef>

namespace chunked_list {
  template<typename T, size_t ChunkSize>
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

    template<template <typename...> typename TemplateT, typename T>
    class is_template_of {
      template<typename>
      class Impl : public std::false_type {};

      template<typename... Args>
      class Impl<TemplateT<Args...> > : public std::true_type {};

      public:
        static constexpr bool value = Impl<T>::value;
    };

    template<typename ChunkedListT, typename IteratorT>
    concept is_iterator = is_template_of<ChunkedListT::template GenericIterator, IteratorT>::value;

    template<typename ChunkedListT, typename... IteratorTs>
    concept are_iterators = (is_iterator<ChunkedListT, IteratorTs> && ...);

    template<typename ChunkedListT, typename ChunkIteratorT>
    concept is_chunk_iterator = is_template_of<ChunkedListT::template GenericChunkIterator, ChunkIteratorT>::value;

    template<typename ChunkedListT, typename... ChunkIteratorTs>
    concept are_chunk_iterators = (is_chunk_iterator<ChunkedListT, ChunkIteratorTs> && ...);

    template<typename ChunkedListT, typename... IteratorTs>
    concept are_iterators_or_chunk_iterators = are_iterators<ChunkedListT, IteratorTs...> ||
      are_chunk_iterators<ChunkedListT, IteratorTs...>;

    template<typename ChunkedListT, typename SliceT>
    concept is_slice = is_template_of<ChunkedListT::template GenericSlice, SliceT>::value;

    template<typename OutputStream, typename T>
    concept can_insert = requires(OutputStream os, T obj) {
      std::is_reference_v<std::decay_t<decltype(os << obj)>>;
      { os << obj };
      std::is_base_of_v<std::remove_reference_t<decltype(os << obj)>, OutputStream>;
    };

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
}

#include "ChunkedListUtility.tpp"
