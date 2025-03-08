#define CHUNKED_LIST_TYPE 2

#include "ChunkedListSnake.hpp"
#include "../core/TestUtility.hpp"

using namespace TestUtility;

int main() {
  BEGIN

  using namespace Tests;

  callFunction("Front and back", [] {
    testData.setSource("Test 1");
    FrontAndBack<Chunked_List, 1>();

    testData.setSource("Test 2");
    FrontAndBack<Chunked_List, 2>();

    testData.setSource("Test 3");
    FrontAndBack<Chunked_List, 3>();

    testData.setSource("Test 4");
    FrontAndBack<Chunked_List, 4>();

    testData.setSource("Test 5");
    FrontAndBack<Chunked_List, 5>();
  });

  callFunction("Insertion", [] {
    testData.setSource("Test 1");
    Insertion<Chunked_List, 1>();

    testData.setSource("Test 2");
    Insertion<Chunked_List, 2>();

    testData.setSource("Test 3");
    Insertion<Chunked_List, 3>();

    testData.setSource("Test 4");
    Insertion<Chunked_List, 4>();

    testData.setSource("Test 5");
    Insertion<Chunked_List, 5>();
  });

  callFunction("Sorting", [] {
    using namespace chunked_list::utility;

    testData.setSource("Bubble Sorting 1");
    Sorting<Chunked_List, BubbleSort, 1>();

    testData.setSource("Bubble Sorting 2");
    Sorting<Chunked_List, BubbleSort, 7>();

    testData.setSource("Bubble Sorting 3");
    Sorting<Chunked_List, BubbleSort, 1000>();

    testData.setSource("Selection Sorting 1");
    Sorting<Chunked_List, SelectionSort, 1>();

    testData.setSource("Selection Sorting 2");
    Sorting<Chunked_List, SelectionSort, 7>();

    testData.setSource("Selection Sorting 3");
    Sorting<Chunked_List, SelectionSort, 1000>();

    testData.setSource("Insertion Sorting 1");
    Sorting<Chunked_List, InsertionSort, 1>();

    testData.setSource("Insertion Sorting 2");
    Sorting<Chunked_List, InsertionSort, 7>();

    testData.setSource("Insertion Sorting 3");
    Sorting<Chunked_List, InsertionSort, 1000>();

    testData.setSource("Quick Sorting 1");
    Sorting<Chunked_List, QuickSort, 1>();

    testData.setSource("Quick Sorting 2");
    Sorting<Chunked_List, QuickSort, 1>();

    testData.setSource("Quick Sorting 3");
    Sorting<Chunked_List, QuickSort, 1>();

    testData.setSource("Heap Sorting 1");
    Sorting<Chunked_List, HeapSort, 1>();

    testData.setSource("Heap Sorting 2");
    Sorting<Chunked_List, HeapSort, 7>();

    testData.setSource("Heap Sorting 3");
    Sorting<Chunked_List, HeapSort, 1000>();
  });

  callFunction("Iterators", [] {
    testData.setSource("Test 1");
    Iterators<Chunked_List, 1>();

    testData.setSource("Test 2");
    Iterators<Chunked_List, 2>();

    testData.setSource("Test 3");
    Iterators<Chunked_List, 3>();

    testData.setSource("Test 4");
    Iterators<Chunked_List, 4>();

    testData.setSource("Test 5");
    Iterators<Chunked_List, 5>();
  });

  callFunction("Pushing and popping", [] {
    testData.setSource("Test 1");
    PushingAndPopping<Chunked_List, 1>();

    testData.setSource("Test 2");
    PushingAndPopping<Chunked_List, 2>();

    testData.setSource("Test 3");
    PushingAndPopping<Chunked_List, 3>();

    testData.setSource("Test 4");
    PushingAndPopping<Chunked_List, 4>();

    testData.setSource("Test 5");
    PushingAndPopping<Chunked_List, 5>();
  });

  callFunction("Equality and inequality", [] {
    testData.setSource("Test 1");
    EqualityAndInequality<Chunked_List, 1>();

    testData.setSource("Test 2");
    EqualityAndInequality<Chunked_List, 2>();

    testData.setSource("Test 3");
    EqualityAndInequality<Chunked_List, 3>();

    testData.setSource("Test 4");
    EqualityAndInequality<Chunked_List, 4>();

    testData.setSource("Test 5");
    EqualityAndInequality<Chunked_List, 5>();
  });

  callFunction("Concatenation and indexing", [] {
    testData.setSource("Test 1");
    ConcatenationAndIndexing<Chunked_List, 1>();

    testData.setSource("Test 2");
    ConcatenationAndIndexing<Chunked_List, 2>();

    testData.setSource("Test 3");
    ConcatenationAndIndexing<Chunked_List, 3>();

    testData.setSource("Test 4");
    ConcatenationAndIndexing<Chunked_List, 4>();

    testData.setSource("Test 5");
    ConcatenationAndIndexing<Chunked_List, 5>();
  });

  SUCCESS
}
