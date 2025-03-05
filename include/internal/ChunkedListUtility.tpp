#pragma once

#include <algorithm>
#include <queue>

#include "internal/ChunkedListUtility.hpp"

using namespace chunked_list_utility;

template<typename Compare, SortType Sort = QuickSort, typename T, size_t ChunkSize>
void chunked_list_utility::sort(ChunkedList<T, ChunkSize> &chunkedList) {
  using namespace sort_functions;

  switch (Sort) {
    case BubbleSort: return bubble_sort<Compare>(chunkedList);
    case SelectionSort: return selection_sort<Compare>(chunkedList);
    case InsertionSort: return insertion_sort<Compare>(chunkedList);
    case QuickSort: return quick_sort<Compare, T, ChunkSize>(chunkedList.begin(), chunkedList.end());
    case HeapSort: return heap_sort<Compare>(chunkedList);
  }
}

template<typename Compare, typename T, size_t ChunkSize>
void sort_functions::bubble_sort(ChunkedList<T, ChunkSize> &chunkedList) {
  if (1 >= chunkedList.size()) return;

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
void sort_functions::selection_sort(ChunkedList<T, ChunkSize> &chunkedList) {
  if (1 >= chunkedList.size()) return;

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
void sort_functions::insertion_sort(ChunkedList<T, ChunkSize> &chunkedList) {
  if (chunkedList.size() <= 1) return;

  Compare compare;

  auto begin = chunkedList.begin(), end = chunkedList.end();

  for (auto startingIt = std::next(begin); startingIt != end; ++startingIt) {
    T value = *startingIt;

    auto it = startingIt;

    while (it != begin) {
      auto prevIt = std::prev(it);

      if (!compare(value, *prevIt)) break; // value < *prevIt

      *it = std::move(*prevIt);
      it = prevIt;
    }

    *it = std::move(value);
  }
}

template<typename Compare, typename T, size_t ChunkSize>
void sort_functions::quick_sort(typename ChunkedList<T, ChunkSize>::Iterator start,
                              typename ChunkedList<T, ChunkSize>::Iterator end) {
  if (start == end || std::next(start) == end) return;

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

    if (left == right || std::next(right) == left) break;

    std::swap(*left, *right);
    ++left;
    --right;
  }

  std::swap(*left, *pivot);

  quick_sort<Compare, T, ChunkSize>(start, left); // Left partition
  quick_sort<Compare, T, ChunkSize>(std::next(left), end); // Right partition
}

template<typename Compare, typename T, size_t ChunkSize>
void sort_functions::heap_sort(ChunkedList<T, ChunkSize> &chunkedList) {
  if (1 >= chunkedList.size()) return;

  std::priority_queue<T, std::vector<T>, Compare> heap{};

  for (auto it = chunkedList.begin(); it != chunkedList.end(); ++it) {
    heap.push(std::move(*it));
  }

  for (auto iterator = chunkedList.end() - 1;; --iterator) {
    *iterator = std::move(heap.top());
    heap.pop();

    if (iterator == chunkedList.begin()) break;
  }
}
