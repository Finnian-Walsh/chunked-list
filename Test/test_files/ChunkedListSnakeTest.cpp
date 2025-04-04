#define CHUNKED_LIST_TYPE 2

#include "TestUtility.hpp"
#include "chunked-list/ChunkedListSnake.hpp"

using namespace TestUtility;

int main() {
  BEGIN

  using namespace Tests;

  callFunction<Chunk, Chunked_List>();

  callFunction<Initialization, ChunkedList>();

  callFunction<Faces, Chunked_List>();

  callFunction<Stacking, Chunked_List>();

  callFunction<Iterators, Chunked_List>();

  callFunction<Clearing, Chunked_List>();

  callFunction<Sorting, Chunked_List>();

  callFunction<SlicesAndIterators, Chunked_List>();

  callFunction<PushingAndPopping, Chunked_List>();

  callFunction<EqualityAndInequality, Chunked_List>();

  callFunction<ConcatenationAndIndexing, Chunked_List>();

  SUCCESS
}
