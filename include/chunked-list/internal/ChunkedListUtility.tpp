#pragma once

#include <algorithm>
#include <queue>

#include "ChunkedListUtility.hpp"

namespace chunked_list::utility {
  template<string_compatible T>
  std::string intoString(T value) {
    if constexpr (can_construct<std::string, T>) {
      return std::string{value};
    }

    if constexpr (can_to_string<T>) {
      return std::to_string(value);
    }

    if constexpr (can_insert<std::ostringstream, T>) {
      std::ostringstream oss;
      oss << value;
      return oss.str();
    }

    throw std::runtime_error("Value can not be converted to string!");
  }

  template<string_compatible... Args>
  std::string concatenate(Args &&...args) {
    return (intoString(std::move(args)) += ...);
  }

  namespace sort_functions {
    template<typename Compare, typename T, size_t ChunkSize>
    void bubble_sort(ChunkedList<T, ChunkSize> &chunkedList) {
      if (1 >= chunkedList.size())
        return;

      Compare compare;

      bool sorted{};

      do {
        sorted = true;

        for (auto it = chunkedList.begin(); std::next(it) != chunkedList.end(); ++it) {
          if (compare(*std::next(it), *it)) {
            sorted = false;
            std::swap(*it, *std::next(it));
          }
        }
      } while (!sorted);
    }

    template<typename Compare, typename T, size_t ChunkSize>
    void selection_sort(ChunkedList<T, ChunkSize> &chunkedList) {
      if (1 >= chunkedList.size())
        return;

      Compare compare;

      auto startingIt = chunkedList.begin();
      const size_t range = chunkedList.size() - 1;

      for (size_t i = 0; i < range; ++i) {
        auto min = startingIt;

        for (auto it = std::next(startingIt); it != chunkedList.end(); ++it) {
          if (compare(*it, *min)) {
            min = it;
          }
        }

        if (min != startingIt) {
          std::swap(*min, *startingIt);
        }

        ++startingIt;
      }
    }

    template<typename Compare, typename T, size_t ChunkSize>
    void insertion_sort(ChunkedList<T, ChunkSize> &chunkedList) {
      if (chunkedList.size() <= 1)
        return;

      Compare compare;

      auto begin = chunkedList.begin(), end = chunkedList.end();

      for (auto startingIt = std::next(begin); startingIt != end; ++startingIt) {
        T value = *startingIt;

        auto it = startingIt;

        while (it != begin) {
          auto prevIt = std::prev(it);

          if (!compare(value, *prevIt))
            break; // value < *prevIt

          *it = std::move(*prevIt);
          it = prevIt;
        }

        *it = std::move(value);
      }
    }

    template<typename Compare, typename T, size_t ChunkSize>
    void quick_sort(typename ChunkedList<T, ChunkSize>::Iterator start,
                    typename ChunkedList<T, ChunkSize>::Iterator end) {
      if (start == end || std::next(start) == end)
        return;

      Compare compare;

      using Iterator = typename ChunkedList<T, ChunkSize>::Iterator;

      Iterator pivot = end;
      --pivot;

      Iterator left = start;
      Iterator right = pivot;

      while (true) {
        while (left != pivot && compare(*left, *pivot)) {
          ++left;
        }

        while (right != start && compare(*pivot, *right)) {
          --right;
        }

        if (left == right || std::next(right) == left)
          break;

        std::swap(*left, *right);
        ++left;
        --right;
      }

      std::swap(*left, *pivot);

      quick_sort<Compare, T, ChunkSize>(start, left); // Left partition
      quick_sort<Compare, T, ChunkSize>(std::next(left), end); // Right partition
    }

    template<typename Compare, typename T, size_t ChunkSize>
    void heap_sort(ChunkedList<T, ChunkSize> &chunkedList) {
      if (1 >= chunkedList.size())
        return;

      std::priority_queue<T, std::vector<T>, Compare> heap{};

      for (auto it = chunkedList.begin(); it != chunkedList.end(); ++it) {
        heap.push(std::move(*it));
      }

      for (auto iterator = chunkedList.end() - 1;; --iterator) {
        *iterator = std::move(heap.top());
        heap.pop();

        if (iterator == chunkedList.begin())
          break;
      }
    }
  } // namespace sort_functions
} // namespace chunked_list::utility
