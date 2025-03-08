#define CHUNKED_LIST_TYPE 1

#include "../core/TestUtility.hpp"

using namespace TestUtility;

int main() {
  BEGIN

  using namespace Tests;

  callFunction("Front and back", [] {
    testData.setSource("Test 1");
    FrontAndBack<ChunkedList, 1>();

    testData.setSource("Test 2");
    FrontAndBack<ChunkedList, 2>();

    testData.setSource("Test 3");
    FrontAndBack<ChunkedList, 3>();

    testData.setSource("Test 4");
    FrontAndBack<ChunkedList, 4>();

    testData.setSource("Test 5");
    FrontAndBack<ChunkedList, 5>();
  });

  callFunction("Insertion", [] {
    testData.setSource("Test 1");
    Insertion<ChunkedList, 1>();

    testData.setSource("Test 2");
    Insertion<ChunkedList, 2>();

    testData.setSource("Test 3");
    Insertion<ChunkedList, 3>();

    testData.setSource("Test 4");
    Insertion<ChunkedList, 4>();

    testData.setSource("Test 5");
    Insertion<ChunkedList, 5>();
  });

  callFunction("Sorting", [] {
    using namespace chunked_list::utility;

    testData.setSource("Bubble Sorting 1");
    Sorting<ChunkedList, BubbleSort, 1>();

    testData.setSource("Bubble Sorting 2");
    Sorting<ChunkedList, BubbleSort, 7>();

    testData.setSource("Bubble Sorting 3");
    Sorting<ChunkedList, BubbleSort, 1000>();

    testData.setSource("Selection Sorting 1");
    Sorting<ChunkedList, SelectionSort, 1>();

    testData.setSource("Selection Sorting 2");
    Sorting<ChunkedList, SelectionSort, 7>();

    testData.setSource("Selection Sorting 3");
    Sorting<ChunkedList, SelectionSort, 1000>();

    testData.setSource("Insertion Sorting 1");
    Sorting<ChunkedList, InsertionSort, 1>();

    testData.setSource("Insertion Sorting 2");
    Sorting<ChunkedList, InsertionSort, 7>();

    testData.setSource("Insertion Sorting 3");
    Sorting<ChunkedList, InsertionSort, 1000>();

    testData.setSource("Quick Sorting 1");
    Sorting<ChunkedList, QuickSort, 1>();

    testData.setSource("Quick Sorting 2");
    Sorting<ChunkedList, QuickSort, 1>();

    testData.setSource("Quick Sorting 3");
    Sorting<ChunkedList, QuickSort, 1>();

    testData.setSource("Heap Sorting 1");
    Sorting<ChunkedList, HeapSort, 1>();

    testData.setSource("Heap Sorting 2");
    Sorting<ChunkedList, HeapSort, 7>();

    testData.setSource("Heap Sorting 3");
    Sorting<ChunkedList, HeapSort, 1000>();
  });

  callFunction("Iterators", [] {
    testData.setSource("Test 1");
    Iterators<ChunkedList, 1>();

    testData.setSource("Test 2");
    Iterators<ChunkedList, 2>();

    testData.setSource("Test 3");
    Iterators<ChunkedList, 3>();

    testData.setSource("Test 4");
    Iterators<ChunkedList, 4>();

    testData.setSource("Test 5");
    Iterators<ChunkedList, 5>();
  });

  callFunction("Pushing and popping", [] {
    testData.setSource("Test 1");
    PushingAndPopping<ChunkedList, 1>();

    testData.setSource("Test 2");
    PushingAndPopping<ChunkedList, 2>();

    testData.setSource("Test 3");
    PushingAndPopping<ChunkedList, 3>();

    testData.setSource("Test 4");
    PushingAndPopping<ChunkedList, 4>();

    testData.setSource("Test 5");
    PushingAndPopping<ChunkedList, 5>();
  });

  callFunction("Equality and inequality", [] {
    testData.setSource("Test 1");
    EqualityAndInequality<ChunkedList, 1>();

    testData.setSource("Test 2");
    EqualityAndInequality<ChunkedList, 2>();

    testData.setSource("Test 3");
    EqualityAndInequality<ChunkedList, 3>();

    testData.setSource("Test 4");
    EqualityAndInequality<ChunkedList, 4>();

    testData.setSource("Test 5");
    EqualityAndInequality<ChunkedList, 5>();
  });

  callFunction("Concatenation and indexing", [] {
    testData.setSource("Test 1");
    ConcatenationAndIndexing<ChunkedList, 1>();

    testData.setSource("Test 2");
    ConcatenationAndIndexing<ChunkedList, 2>();

    testData.setSource("Test 3");
    ConcatenationAndIndexing<ChunkedList, 3>();

    testData.setSource("Test 4");
    ConcatenationAndIndexing<ChunkedList, 4>();

    testData.setSource("Test 5");
    ConcatenationAndIndexing<ChunkedList, 5>();
  });

  SUCCESS
}
