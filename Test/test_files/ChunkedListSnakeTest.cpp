#define CHUNKED_LIST_TYPE 2

#include "ChunkedListSnake.hpp"
#include "../core/TestUtility.hpp"

using namespace TestUtility;

int main() {
  BEGIN

  using namespace Tests;

  callFunction("Front and back", [] {
    testData.setSource("Test 1");
    FrontAndBack<chunked_list, 1>();

    testData.setSource("Test 2");
    FrontAndBack<chunked_list, 2>();

    testData.setSource("Test 3");
    FrontAndBack<chunked_list, 3>();

    testData.setSource("Test 4");
    FrontAndBack<chunked_list, 4>();

    testData.setSource("Test 5");
    FrontAndBack<chunked_list, 5>();
  });

  callFunction("Insertion", [] {
    testData.setSource("Test 1");
    Insertion<chunked_list, 1>();

    testData.setSource("Test 2");
    Insertion<chunked_list, 2>();

    testData.setSource("Test 3");
    Insertion<chunked_list, 3>();

    testData.setSource("Test 4");
    Insertion<chunked_list, 4>();

    testData.setSource("Test 5");
    Insertion<chunked_list, 5>();
  });

  callFunction("Sorting", [] {
    testData.setSource("Bubble Sorting 1");
    Sorting<chunked_list, BubbleSort, 1>();

    testData.setSource("Bubble Sorting 2");
    Sorting<chunked_list, BubbleSort, 7>();

    testData.setSource("Bubble Sorting 3");
    Sorting<chunked_list, BubbleSort, 1000>();

    testData.setSource("Selection Sorting 1");
    Sorting<chunked_list, SelectionSort, 1>();

    testData.setSource("Selection Sorting 2");
    Sorting<chunked_list, SelectionSort, 7>();

    testData.setSource("Selection Sorting 3");
    Sorting<chunked_list, SelectionSort, 1000>();

    testData.setSource("Insertion Sorting 1");
    Sorting<chunked_list, InsertionSort, 1>();

    testData.setSource("Insertion Sorting 2");
    Sorting<chunked_list, InsertionSort, 7>();

    testData.setSource("Insertion Sorting 3");
    Sorting<chunked_list, InsertionSort, 1000>();

    testData.setSource("Quick Sorting 1");
    Sorting<chunked_list, QuickSort, 1>();

    testData.setSource("Quick Sorting 2");
    Sorting<chunked_list, QuickSort, 1>();

    testData.setSource("Quick Sorting 3");
    Sorting<chunked_list, QuickSort, 1>();

    testData.setSource("Heap Sorting 1");
    Sorting<chunked_list, HeapSort, 1>();

    testData.setSource("Heap Sorting 2");
    Sorting<chunked_list, HeapSort, 7>();

    testData.setSource("Heap Sorting 3");
    Sorting<chunked_list, HeapSort, 1000>();
  });

  callFunction("Iterators", [] {
    testData.setSource("Test 1");
    Iterators<chunked_list, 1>();

    testData.setSource("Test 2");
    Iterators<chunked_list, 2>();

    testData.setSource("Test 3");
    Iterators<chunked_list, 3>();

    testData.setSource("Test 4");
    Iterators<chunked_list, 4>();

    testData.setSource("Test 5");
    Iterators<chunked_list, 5>();
  });

  callFunction("Pushing and popping", [] {
    testData.setSource("Test 1");
    PushingAndPopping<chunked_list, 1>();

    testData.setSource("Test 2");
    PushingAndPopping<chunked_list, 2>();

    testData.setSource("Test 3");
    PushingAndPopping<chunked_list, 3>();

    testData.setSource("Test 4");
    PushingAndPopping<chunked_list, 4>();

    testData.setSource("Test 5");
    PushingAndPopping<chunked_list, 5>();
  });

  callFunction("Equality and inequality", [] {
    testData.setSource("Test 1");
    EqualityAndInequality<chunked_list, 1>();

    testData.setSource("Test 2");
    EqualityAndInequality<chunked_list, 2>();

    testData.setSource("Test 3");
    EqualityAndInequality<chunked_list, 3>();

    testData.setSource("Test 4");
    EqualityAndInequality<chunked_list, 4>();

    testData.setSource("Test 5");
    EqualityAndInequality<chunked_list, 5>();
  });

  callFunction("Concatenation and indexing", [] {
    testData.setSource("Test 1");
    ConcatenationAndIndexing<chunked_list, 1>();

    testData.setSource("Test 2");
    ConcatenationAndIndexing<chunked_list, 2>();

    testData.setSource("Test 3");
    ConcatenationAndIndexing<chunked_list, 3>();

    testData.setSource("Test 4");
    ConcatenationAndIndexing<chunked_list, 4>();

    testData.setSource("Test 5");
    ConcatenationAndIndexing<chunked_list, 5>();
  });

  SUCCESS
}
