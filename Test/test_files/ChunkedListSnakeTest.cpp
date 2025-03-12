#define CHUNKED_LIST_TYPE 2

#include "ChunkedListSnake.hpp"
#include "../core/TestUtility.hpp"

using namespace TestUtility;

int main() {
  BEGIN

  using namespace Tests;

  callFunction<FrontAndBack, Chunked_List>("Front and back");

  callFunction<Insertion, Chunked_List>("Insertion");

  callFunction<Sorting, Chunked_List>("Sorting");

  callFunction<SlicesAndIterators, Chunked_List>("Slices and iterators");

  callFunction<PushingAndPopping, Chunked_List>("Pushing and popping");

  callFunction<EqualityAndInequality, Chunked_List>("Equality and inequality");

  callFunction<ConcatenationAndIndexing, Chunked_List>("Concatenation and indexing");

  SUCCESS
}
