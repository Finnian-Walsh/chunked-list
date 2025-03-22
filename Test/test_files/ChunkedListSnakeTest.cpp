#define CHUNKED_LIST_TYPE 2

#include "chunked-list/ChunkedListSnake.hpp"
#include "TestUtility.hpp"

using namespace TestUtility;

int main() {
  BEGIN

  using namespace Tests;

  callFunction<Initialization, ChunkedList>("Initialization");

  callFunction<Chunk, Chunked_List>("Chunk");

  callFunction<ListEnds, Chunked_List>("List ends");

  callFunction<Insertion, Chunked_List>("Insertion");

  callFunction<Sorting, Chunked_List>("Sorting");

  callFunction<SlicesAndIterators, Chunked_List>("Slices and iterators");

  callFunction<PushingAndPopping, Chunked_List>("Pushing and popping");

  callFunction<EqualityAndInequality, Chunked_List>("Equality and inequality");

  callFunction<ConcatenationAndIndexing, Chunked_List>("Concatenation and indexing");

  SUCCESS
}
