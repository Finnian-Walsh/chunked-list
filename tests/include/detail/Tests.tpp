
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
    chunkedList.push_back(num);
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
      list.push_back(static_cast<IntegralT>(i));
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
  list.push_back(NUM);

  const IntegralT first = list[0];

  THROW_IF(NUM != first, concatenate("Expected first item in list to be ", NUM, " but received ", first))

  THROW_IF(list.size() != 1, concatenate(""))
}

template<typename CLT>
void Tests::Sorting<CLT>::operator()() const {
  using namespace utility;

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
template<utility::SortType SortingAlgorithm>
void Tests::Sorting<CLT>::sort() const {
  performTask("List creation");
  CLT list;

  performTask("Random Number Generator creation");
  RandomNumberGenerator rng;

  performTask("Pushing or using the RNG");
  static constexpr int ITERATIONS = 300;
  for (int i = 0; i < ITERATIONS; ++i) {
    list.push_back(rng(std::numeric_limits<IntegralT>::min(), std::numeric_limits<IntegralT>::max()));
  }

  performTask("Sorting");
  list.template sort<std::less<IntegralT>, SortingAlgorithm>();

  performTask("Indexing");
  int last = list[0];

  performTask("Calling begin()");
  auto iterator = list.begin();

  performTask("iterator operator+=()");
  iterator += 1;

  performTask("Calling end()");
  const auto end = list.end();

  performTask("iterator inequality");
  for (; iterator != end; ++iterator) {
    performTask("iterator dereferencing");
    THROW_IF(*iterator < last, "Sorting failed!")

    last = *iterator;

    performTask("iterator operator++(int)");
  }
}

template<typename CLT>
void Tests::PushingAndPopping<CLT>::operator()() const {
  CLT chunkedList{};

  performTask("Pushing");
  for (int i = 0; i < 10; ++i) {
    chunkedList.push_back('a');
    chunkedList.push_back('b');
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
    chunkedList.push_back(static_cast<IntegralT>(i));
  }

  const CLT &constListRef = chunkedList;
  {
    performTask("Slicing (1)");
    RandomNumberGenerator<size_t> rng;

    const size_t startOffset = rng(0, MAX - 2);
    const size_t endOffset = rng(startOffset, MAX - 1);

    auto it1 = begin(chunkedList) + startOffset, it2 = begin(chunkedList) + endOffset;

    auto slice = chunkedList.slice(it1, it2);

    auto slice2 = constListRef.slice(it1, it2);

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

    for (auto slice = chunkedList.slice(startIndex, endIndex); auto item : slice) {
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
    auto beginChunk = chunkedList.template begin<CLT::chunk_iterator>();
    auto constBeginChunk = constListRef.template begin<CLT::chunk_iterator>();

    THROW_IF(beginChunk != constBeginChunk, "ChunkIterator is unequal to const ChunkIterator")
    THROW_IF(constBeginChunk != beginChunk, "const ChunkIterator is unequal to ChunkIterator")

    THROW_IF(!(beginChunk == constBeginChunk), "ChunkIterator is not equal to const ChunkIterator")
    THROW_IF(!(beginChunk == constBeginChunk), "const ChunkIterator is not equal to ChunkIterator")

    THROW_IF(std::next(beginChunk) != beginChunk + 1, "std::next(ChunkIterator) is not equal to ChunkIterator + 1")
    int tracker = 1;

    for (auto chunkIt = beginChunk; chunkIt != chunkedList.template end<CLT::chunk_iterator>(); ++chunkIt) {
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
    chunkedList.push_back(i);
  }
  {
    performTask("Calling begin method with chunk iterator");
    typename CLT::chunk_iterator chunkIt = chunkedList.begin();
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
    chunkedList.push_back(i);
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
    list1.push_back(i);
    list2.push_back(i);
  }

  list1.push_back(4);
  list2.push_back(3);

  THROW_IF(list1 == list2, "List comparison 1 failed")

  performTask("Popping");
  list2.pop();

  performTask("Pushing");
  list2.push_back(4);

  performTask("ChunkedList inequality");
  THROW_IF(list1 != list2, "List comparison 2 failed")

  performTask("Pushing");
  for (int i = 0; i < 80; ++i) {
    list1.push_back(i);
    list2.push_back(i);
  }

  list1.push_back(1);
  list2.push_back(1);

  performTask("ChunkedList inequality");
  THROW_IF(list1 != list2, "List comparison 3 failed")

  performTask("Popping");
  list2.pop();

  performTask("Pushing");
  list2.push_back(2);

  performTask("List equality");
  THROW_IF(list1 == list2, "List comparison 4 failed")
}
