#define CHUNKED_LIST_TYPE 1

#include "TestUtility.hpp"

using namespace TestUtility;

int main() {
  BEGIN

  using namespace Tests;

  callFunction<Chunk, ChunkedList>();

  callFunction<Initialization, ChunkedList>();

  callFunction<Edges, ChunkedList>();

  callFunction<Insertion, ChunkedList>();

  callFunction<Sorting, ChunkedList>();

  callFunction<SlicesAndIterators, ChunkedList>();

  callFunction<PushingAndPopping, ChunkedList>();

  callFunction<EqualityAndInequality, ChunkedList>();

  callFunction<ConcatenationAndIndexing, ChunkedList>();

  SUCCESS
}
