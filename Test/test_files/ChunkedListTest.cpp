#define CHUNKED_LIST_TYPE 1

#include "../core/TestUtility.hpp"

using namespace TestUtility;

int main() {
  BEGIN

  using namespace Tests;

  callFunction<FrontAndBack, ChunkedList>("Front and back");

  callFunction<Insertion, ChunkedList>("Insertion");

  callFunction<Sorting, ChunkedList>("Sorting");

  callFunction<SlicesAndIterators, ChunkedList>("Slices and iterators");

  callFunction<PushingAndPopping, ChunkedList>("Pushing and popping");

  callFunction<EqualityAndInequality, ChunkedList>("Equality and inequality");

  callFunction<ConcatenationAndIndexing, ChunkedList>("Concatenation and indexing");

  SUCCESS
}
