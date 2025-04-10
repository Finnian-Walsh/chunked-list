#pragma once

#include "TestUtility.hpp"

template<typename CLT>
void Tests::Chunk<CLT>::operator()() const {
  using AccessorType = Accessor<CLT>;
  using ChunkType = typename AccessorType::Chunk;

  using ComplexChunkType =
    typename ChunkedListAccessor<ComplexT, CLT::chunk_size * 2, typename CLT::allocator_type>::Chunk;

  initialization<ChunkType>();
  indexing<ChunkType>();
  stacking<ComplexChunkType>();
  arithmetic<ChunkType>();
  size<ChunkType>();
  comparison<ChunkType>();
  iteration<ChunkType>();
}

template<typename CLT>
template<typename ChunkT>
void Tests::Chunk<CLT>::initialization() {
  std::vector<IntegralT> vec;

  for (size_t index = 0; index < ChunkT::chunk_size; ++index) {
    vec.push_back(static_cast<IntegralT>(index));
  }

  ChunkT *ptr = nullptr;
  IntegralT value{};

  ChunkT chunk1;
  ChunkT chunk2{ptr, ptr};
  ChunkT chunk3{ptr};

  ChunkT chunk4{vec.data(), vec.size(), ptr, ptr};
  ChunkT chunk5{vec.data(), vec.size(), ptr};
  ChunkT chunk6{vec.data(), vec.size()};

  ChunkT chunk7{value, ptr, ptr};
  ChunkT chunk8{value, ptr};
  ChunkT chunk9{value};
}

template<typename CLT>
template<typename LCT>
void Tests::Chunk<CLT>::indexing() {
  performTask("Chunk indexing");
  std::vector<IntegralT> vec;

  for (size_t i = 0; i < LCT::chunk_size; ++i) {
    vec.push_back(static_cast<IntegralT>(i));
  }

  LCT chunk{vec.data(), vec.size()};

  for (size_t i = 0; i < LCT::chunk_size; ++i) {
    const std::string &&errMessage =
      concatenate("Expected ", ordinalize(i), " element of chunked list to be equal to ", i);
    THROW_IF(chunk[i] != vec[i], std::move(errMessage))
    THROW_IF(chunk.at(i) != vec.at(i), std::move(errMessage))
  }
}

template<typename CLT>
template<typename CT>
void Tests::Chunk<CLT>::stacking() {
  performTask("Stacking");

  constexpr size_t CHUNK_SIZE = CT::chunk_size;

  constexpr size_t HALF_CHUNK_SIZE = CHUNK_SIZE / 2;

  CT chunk;

  using ValueType = typename CT::value_type;

  std::vector<ValueType> vec;

  for (size_t i = 0; i < HALF_CHUNK_SIZE; ++i) {
    const IntegralT num = static_cast<IntegralT>(i);
    chunk.push({num, num});
    vec.push_back({num, num});
  }

  for (size_t i = HALF_CHUNK_SIZE; i < CHUNK_SIZE; ++i) {
    const IntegralT num = static_cast<IntegralT>(i);
    chunk.emplace(num, num);
    vec.emplace_back(num, num);
  }

  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    THROW_IF(chunk[i] != vec[i], concatenate("Expected item to be equal to ", vec[i]))
  }
}

template<typename CLT>
template<typename CT>
void Tests::Chunk<CLT>::arithmetic() {
  performTask("Chunk advancing");

  CustomAllocator<CT> allocator;

  CT *front = allocator.allocate(1);
  CT *back = front;

  allocator.construct(front);

  constexpr size_t RANGE = 3;

  // Allocation

  for (size_t i = 0; i < RANGE; ++i) {
    CT *next = allocator.allocate(1);
    allocator.construct(next, nullptr, 0, back);
    back->nextChunk = next;
    back = next;
  }

  CT *expectedBack = &((*front) + RANGE);

  THROW_IF(expectedBack != back, concatenate("Expected front + ", RANGE, " to be equal to back"))

  CT *expectedFront = &((*back) - RANGE);

  THROW_IF(expectedFront != front, concatenate("Expected front + ", RANGE, " to be equal to front"))

  // Deallocation

  size_t counter{};

  do {
    CT *prev = back->prevChunk;
    allocator.destroy(back);
    allocator.deallocate(back, 1);
    back = prev;
    ++counter;
  } while (back); // repeats RANGE + 1 times

  constexpr size_t EXPECTED_DELETIONS = RANGE + 1;
  THROW_IF(counter != EXPECTED_DELETIONS,
           concatenate("Expected ", EXPECTED_DELETIONS, " deletions, but there were ", counter))
}

template<typename CLT>
template<typename CT>
void Tests::Chunk<CLT>::size() {
  constexpr size_t chunk_size = CT::chunk_size;

  for (size_t loadPercentage = 0; loadPercentage <= 100; loadPercentage += 10) {
    const size_t load = chunk_size * loadPercentage / 100;

    std::vector<IntegralT> vec;

    for (IntegralT i = 0; i < static_cast<IntegralT>(load); ++i) {
      vec.push_back(i);
    }

    CT chunk{vec.data(), vec.size()};

    const size_t chunkSize = chunk.size();
    THROW_IF(chunkSize != load, concatenate("Expected chunk size to be equal to ", load, " but received ", chunk_size))
  }

  CT chunk;

  THROW_IF(!chunk.empty(), "Expected empty chunk to be empty")

  for (size_t i = 0; i < chunk_size; ++i) {
    chunk.emplace();
  }

  for (size_t i = 0; i < chunk_size; ++i) {
    chunk.pop();
  }

  THROW_IF(!chunk.empty(), concatenate("Expected chunk to be empty, but its size is ", chunk.size()))
}

template<typename CLT>
template<typename CT>
void Tests::Chunk<CLT>::comparison() {
  CT chunkA;
  CT chunkB;

  THROW_IF(chunkA == chunkB, "chunk a equal to chunk b")
  THROW_IF(!(chunkA != chunkB), "chunk a not unequal to chunk b")
  THROW_IF(chunkA != chunkA, "chunk a unequal to chunk a")
  THROW_IF(!(chunkA == chunkA), "chunk a not equal to chunk a")
}

template<typename CLT>
template<typename CT>
void Tests::Chunk<CLT>::iteration() {
  performTask("Chunk iteration");
  constexpr size_t CHUNK_SIZE = CT::chunk_size;

  std::vector<IntegralT> vec;
  vec.reserve(CHUNK_SIZE);

  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    vec.push_back(static_cast<IntegralT>(i));
  }

  CT chunk{vec.data(), CHUNK_SIZE};

  IntegralT expected = 0;
  for (auto it = chunk.begin(); it != chunk.end(); ++it) {
    auto item = *it;
    THROW_IF(item != expected, concatenate("Expected item to be equal to ", expected, " but received ", item))
    ++expected;
  }

  THROW_IF(static_cast<size_t>(expected) != CHUNK_SIZE,
           concatenate("Expected 'expected' to finish at ", CHUNK_SIZE, " but received ", expected))
}

template<typename CLT>
void Tests::Initialization<CLT>::operator()() const {
  using AccessorType = Accessor<CLT>;

  default_construction<AccessorType>();
  initializer_list_construction<AccessorType>();
}

template<typename CLT>
template<typename AT>
void Tests::Initialization<CLT>::default_construction() {
  performTask("Constructing list with default initializer");
  CLT list;

  performTask("Reinterpreting the memory address of a ChunkedList as a ChunkedListAccessor pointer");
  AT *accessor = reinterpret_cast<AT *>(&list);

  performTask("Calling accessor methods");
  THROW_IF(!accessor->front, "Expected ChunkedList to have front")
  THROW_IF(accessor->front != accessor->back, "Expected front and back to be equal in empty ChunkedList")
}

template<typename CLT>
template<typename AT>
void Tests::Initialization<CLT>::initializer_list_construction() {
  performTask("Constructing list with initializer list");
  CLT list{1, 2}; //, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  AT *accessor = reinterpret_cast<AT *>(&list);
  THROW_IF(!accessor->back, "Expected ChunkedList to have back")
  THROW_IF(!accessor->front, "Expected ChunkedList to have front")
}

template<typename CLT>
void Tests::Faces<CLT>::operator()() {
  performTask("Creating chunked list with initializer list");

  CLT chunkedList{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  using AccessorType = Accessor<CLT>;

  AccessorType *accessor = reinterpret_cast<AccessorType *>(&chunkedList);

  auto *front = accessor->front;
  auto *back = accessor->back;

  THROW_IF((*front)[0] != 1, "First item is not 1")
  THROW_IF((*back)[back->size() - 1] != 10, "Last item is not 10")

  performTask("Calling size()");
  const size_t size = chunkedList.size();

  THROW_IF(size != 10, concatenate("Unexpected size; received ", size, " but expected 10"));

  performTask("Indexing");
  THROW_IF(chunkedList[size - 1] != 10, "Last item is not 10")
}

template<typename CLT>
void Tests::Stacking<CLT>::operator()() {
  CLT chunkedList{5, 10, 15};
  std::string expectedOutput{"[5, 10, 15"};

  RandomNumberGenerator rng;

  constexpr int RANGE = 10;

  for (int i = 0; i < RANGE; ++i) {
    int num = rng(-RANGE, RANGE);
    expectedOutput += ", ";
    expectedOutput += std::to_string(num);
    chunkedList.push(num);
  }

  expectedOutput.push_back(']');

  performTask("Inserting ChunkedList into ostream");
  std::ostringstream oss{};
  oss << chunkedList;

  const std::string str = oss.str();

  THROW_IF(str != expectedOutput,
           concatenate("ostream insertion ran incorrectly\nReceived: ", str, "\nExpected: ", expectedOutput))

  auto i = begin(chunkedList);
}

template<typename CLT>
void Tests::Iterators<CLT>::operator()() const {
  CLT list{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  auto compBeg = begin(list);
  auto memBeg = list.begin();

  THROW_IF(compBeg != memBeg, "begin(list) and list.begin() unequal!")
  THROW_IF(!(compBeg == memBeg), "begin(list) and list.begin() not equal!")

  constexpr bool begEq = std::same_as<decltype(compBeg), decltype(memBeg)>;

  THROW_IF(!begEq, "begin(list) returned a different type to list.begin()!")

  auto compEnd = end(list);
  auto memEnd = list.end();

  THROW_IF(compEnd != memEnd, "end(list) and list.end() unequal!")
  THROW_IF(!(compEnd == memEnd), "end(list) and list.end() not equal!")

  constexpr bool endEq = std::same_as<decltype(compEnd), decltype(memEnd)>;

  THROW_IF(!endEq, "end(list) returned a different type to list.end()!")

  IntegralT expectedValue = 1;

  for (auto it = memBeg; it != memEnd; ++it) {
    auto val = *it;
    THROW_IF(val != expectedValue,
             concatenate("Expected iterator value to be equal to ", expectedValue, " but received ", val))
    ++expectedValue;
  }

  constexpr IntegralT expectedExpectedValue = 17;
  THROW_IF(expectedValue != expectedExpectedValue,
           concatenate("Expected 'expectedValue' to be ", expectedExpectedValue, ", but received ", expectedValue))
}

template<typename CLT>
void Tests::Clearing<CLT>::operator()() const {
  clear<true>();
  clear<false>();
}

template<typename CLT>
template<bool ClearFront>
void Tests::Clearing<CLT>::clear() {
  CLT list;
  constexpr size_t CHUNK_SIZE = CLT::chunk_size;

  for (size_t multiplier = 1; multiplier <= 5; ++multiplier) {
    for (size_t i = 0; i < CHUNK_SIZE * multiplier; ++i) {
      list.push(static_cast<IntegralT>(i));
    }

    list.template clear<ClearFront>();

    postliminary_checks(list);
  }
}

template<typename CLT>
void Tests::Clearing<CLT>::postliminary_checks(CLT &list) {
  const size_t size = list.size();
  THROW_IF(size != 0, concatenate("Expected cleared list to have size of 0 but size() returned ", size))

  constexpr IntegralT NUM = 69;
  list.push(NUM);

  const IntegralT first = list[0];

  THROW_IF(NUM != first, concatenate("Expected first item in list to be ", NUM, " but received ", first))

  THROW_IF(list.size() != 1, concatenate(""))
}

template<typename CLT>
void Tests::Sorting<CLT>::operator()() const {
  const std::string initialSource = testData.getSource();

#define EXTEND_SOURCE(extension) testData.setSource(std::move(concatenate(initialSource, extension)));

  EXTEND_SOURCE(" bubble sort")
  sort<BubbleSort>();

  EXTEND_SOURCE(" selection sort")
  sort<SelectionSort>();

  EXTEND_SOURCE(" insertion sort")
  sort<InsertionSort>();

  EXTEND_SOURCE(" quick sort")
  sort<QuickSort>();

  EXTEND_SOURCE(" heap sort")
  sort<HeapSort>();

#undef EXTEND_SOURCE
}

template<typename CLT>
template<SortType SortingAlgorithm>
void Tests::Sorting<CLT>::sort() const {
  performTask("List creation");
  CLT list;

  performTask("Random Number Generator creation");
  RandomNumberGenerator rng;

  performTask("Pushing or using the RNG");
  static constexpr int ITERATIONS = 300;
  for (int i = 0; i < ITERATIONS; ++i) {
    list.push(rng(std::numeric_limits<IntegralT>::min(), std::numeric_limits<IntegralT>::max()));
  }

  performTask("Sorting");
  list.template sort<std::less<IntegralT>, SortingAlgorithm>();

  performTask("Indexing");
  int last = list[0];

  performTask("Calling begin()");
  auto iterator = list.begin();

  performTask("Iterator operator+=()");
  iterator += 1;

  performTask("Calling end()");
  const auto end = list.end();

  performTask("Iterator inequality");
  for (; iterator != end; ++iterator) {
    performTask("Iterator dereferencing");
    THROW_IF(*iterator < last, "Sorting failed!")

    last = *iterator;

    performTask("Iterator operator++(int)");
  }
}

template<typename CLT>
void Tests::PushingAndPopping<CLT>::operator()() const {
  CLT chunkedList{};

  performTask("Pushing");
  for (int i = 0; i < 10; ++i) {
    chunkedList.push('a');
    chunkedList.push('b');
  }

  for (size_t i = 20; i > 1; --i) {
    performTask("Calling size()");
    THROW_IF(i != chunkedList.size(), "Unexpected ChunkedList size")

    performTask("Popping");
    chunkedList.pop();
  }

  performTask("Indexing");
  const char firstItem = chunkedList[0];

  performTask("Comparing first item");
  THROW_IF(firstItem != 'a', concatenate("First item is ", firstItem, " and not 'a'!"))
}

template<typename CLT>
void Tests::SlicesAndIterators<CLT>::operator()() const {
  performTask("List creation");
  CLT chunkedList;

  constexpr size_t MAX = 10;

  performTask("Pushing");
  for (size_t i = 1; i <= MAX; ++i) {
    chunkedList.push(static_cast<IntegralT>(i));
  }

  const CLT &constListRef = chunkedList;
  {
    performTask("Slicing (1)");
    RandomNumberGenerator<size_t> rng;

    const size_t startOffset = rng(0, MAX - 2);
    const size_t endOffset = rng(startOffset, MAX - 1);

    auto it1 = begin(chunkedList) + startOffset, it2 = begin(chunkedList) + endOffset;

    auto slice =
      chunkedList.VARIANT_CODE(slice, get_slice)(it1, it2);

    auto slice2 =
      constListRef.VARIANT_CODE(slice, get_slice)(it1, it2);

    THROW_IF(slice != slice2, "");

    for (size_t index = 0; index < endOffset - startOffset; ++index) {
      const IntegralT expected = static_cast<IntegralT>(startOffset + index + 1);
      THROW_IF(slice[index] != expected,
               (ordinalize(index) += " item in slice is not equal to ") += std::to_string(expected));
    }

    IntegralT expected = static_cast<IntegralT>(startOffset + 1);
    for (const int num : slice) {
      THROW_IF(num != expected, concatenate("Expected ", expected, " but received ", num))
      ++expected;
    }

    const IntegralT expectedExpected = static_cast<IntegralT>(endOffset + 1);
    THROW_IF(expected != expectedExpected,
             concatenate("Expected 'expected' to be equal to ", expectedExpected, " but received ", expected));
  }
  {
    performTask("Slicing (2)");
    RandomNumberGenerator<size_t> rng;
    const size_t startIndex = rng(0, MAX - 2);
    const size_t endIndex = rng(startIndex, MAX - 1);

    IntegralT expected = static_cast<IntegralT>(startIndex + 1);

    for (auto slice = chunkedList.VARIANT_CODE(slice, get_slice)(startIndex, endIndex); auto item : slice) {
      THROW_IF(item != expected, concatenate("Expected item to be equal to ", expected, " but received ", item));
      ++expected;
    }

    const IntegralT expectedExpected = static_cast<IntegralT>(endIndex + 1);
    THROW_IF(expected != expectedExpected,
             concatenate("Expected 'expected' to be equal to ", expectedExpected, " but got ", expected))
  }
  {
    int total{};

    performTask("List iteration (implicit)");
    for (const int num : chunkedList)
      total += num;

    THROW_IF(total != 55, concatenate("List sum is not 55; received ", total))
  }
  {
    auto begin = chunkedList.begin();
    auto constBegin = constListRef.begin();
    auto it2 = std::next(begin);

    THROW_IF(it2 != begin + 1, "std::next(Iterator) is unequal to Iterator + 1");
    THROW_IF(!(it2 == begin + 1), "std::next(Iterator) is not equal to Iterator + 1")
    THROW_IF(std::prev(it2) != begin, "std::prev(std::next(Iterator)) unequal to Iterator");
    THROW_IF(it2 - 1 != begin, "std::next(Iterator) - 1 is unequal to Iterator")

    THROW_IF(begin != constBegin, "Iterator is unequal to const Iterator")
    THROW_IF(constBegin != begin, "const Iterator is unequal to Iterator")
    THROW_IF(it2 != std::next(constBegin), "std::next(Iterator) is unequal to std::next(const Iterator)")

    constexpr size_t OFFSET = 9;
    THROW_IF(begin + OFFSET != constBegin + OFFSET,
             concatenate("Iterator + ", OFFSET, " is unequal to const Iterator + ", OFFSET));
  }
  {
    auto beginChunk = chunkedList.VARIANT_CODE(beginChunk, begin_chunk)();
    auto constBeginChunk = constListRef.VARIANT_CODE(beginChunk, begin_chunk)();

    THROW_IF(beginChunk != constBeginChunk, "ChunkIterator is unequal to const ChunkIterator")
    THROW_IF(constBeginChunk != beginChunk, "const ChunkIterator is unequal to ChunkIterator")

    THROW_IF(!(beginChunk == constBeginChunk), "ChunkIterator is not equal to const ChunkIterator")
    THROW_IF(!(beginChunk == constBeginChunk), "const ChunkIterator is not equal to ChunkIterator")

    THROW_IF(std::next(beginChunk) != beginChunk + 1, "std::next(ChunkIterator) is not equal to ChunkIterator + 1")
    int tracker = 1;

    for (auto chunkIt = beginChunk; chunkIt != chunkedList.VARIANT_CODE(endChunk, end_chunk)(); ++chunkIt) {
      performTask("ChunkIterator dereferencing");
      auto &chunkRef = *chunkIt;
      performTask("Chunk indexing");
      for (size_t i = 0; i < chunkRef.size(); ++i) {
        IntegralT data = chunkRef[i];
        THROW_IF(data != tracker, concatenate("Invalid number; expected ", tracker, " but received ", data))
        ++tracker;
      }
    }
  }

  performTask("Pushing");
  for (int i = 11; i <= 1000; ++i) {
    chunkedList.push(i);
  }
  {
    performTask(concatenate("Calling ", VARIANT_CODE("beginChunk", "begin_chunk"), " method").c_str());
    auto chunkIt = chunkedList.VARIANT_CODE(beginChunk, begin_chunk)();
    auto chunkIt2 = chunkIt + 1;

    THROW_IF(chunkIt2 - 1 != chunkIt, "ChunkIterator + 1 - 1 is unequal to ChunkIterator")
    THROW_IF(chunkIt2 != std::next(chunkIt), "ChunkIterator + 1 is unequal to std::next(ChunkIterator)")
    THROW_IF(std::prev(chunkIt2) != chunkIt, "std::prev(ChunkIterator + 1) is unequal to ChunkIterator")
  }
}

template<typename CLT>
void Tests::ConcatenationAndIndexing<CLT>::operator()() const {
  CLT chunkedList;

  performTask("Concatenating");
  {
    const std::string result = chunkedList.concat("\b12u3981u283");

    THROW_IF(!result.empty(), "Concatenation failed; expected \"\" but got \"" + result + '\"');
  }

  performTask("Pushing");
  for (int i = 0; i < 10; ++i) {
    chunkedList.push(i);
  }
  {
    performTask("Concatenation");
    std::string &&str = chunkedList.concat(" ");
    THROW_IF(str != "0 1 2 3 4 5 6 7 8 9",
             concatenate("Concatenation failed; expected \"0 1 2 3 4 5 6 7 8 9\" but received \"", str, '"'))
  }

  performTask("Indexing");
  for (int i = 0; i < 10; ++i) {
    const int num = chunkedList[i];
    THROW_IF(num != i, concatenate("Index ", i, " was expected to be ", i, " but received ", num));
  }
}

template<typename CLT>
void Tests::ListComparison<CLT>::operator()() const {
  CLT list1{};
  CLT list2{};

  performTask("Pushing");

  for (int i = 1; i < 4; ++i) {
    list1.push(i);
    list2.push(i);
  }

  list1.push(4);
  list2.push(3);

  THROW_IF(list1 == list2, "List comparison 1 failed")

  performTask("Popping");
  list2.pop();

  performTask("Pushing");
  list2.push(4);

  performTask("ChunkedList inequality");
  THROW_IF(list1 != list2, "List comparison 2 failed")

  performTask("Pushing");
  for (int i = 0; i < 80; ++i) {
    list1.push(i);
    list2.push(i);
  }

  list1.push(1);
  list2.push(1);

  performTask("ChunkedList inequality");
  THROW_IF(list1 != list2, "List comparison 3 failed")

  performTask("Popping");
  list2.pop();

  performTask("Pushing");
  list2.push(2);

  performTask("List equality");
  THROW_IF(list1 == list2, "List comparison 4 failed")
}
