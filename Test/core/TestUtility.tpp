#pragma once

#include "TestUtility.hpp"
#include "ChunkedListAccessor.hpp"

#include <ranges>
#include <cstdlib>
#include <list>
#include <memory>
#include <new>

using namespace TestUtility;

inline void *operator new(const std::size_t size) {
  void *ptr = std::malloc(size);

  if (ptr == nullptr) {
    throw std::bad_alloc();
  }

  if (monitoringAllocations) {
    monitoringAllocations = false;
    allocatedPointers.insert(ptr);
    monitoringAllocations = true;
  }

  return ptr;
}

inline void *operator new(const std::size_t size, const std::nothrow_t &) noexcept {
  void *ptr = std::malloc(size);

  if (ptr == nullptr) {
    return nullptr;
  }

  if (monitoringAllocations) {
    monitoringAllocations = false;
    allocatedPointers.insert(ptr);
    monitoringAllocations = true;
  }

  return ptr;
}

inline void *operator new[](const std::size_t size) {
  void *ptr = std::malloc(size);

  if (ptr == nullptr) {
    throw std::bad_alloc();
  }

  if (monitoringAllocations) {
    monitoringAllocations = false;
    allocatedPointers.insert(ptr);
    monitoringAllocations = true;
  }

  return ptr;
}

inline void *operator new[](const std::size_t size, const std::nothrow_t &) noexcept {
  void *ptr = std::malloc(size);

  if (ptr == nullptr) {
    return nullptr;
  }

  if (monitoringAllocations) {
    monitoringAllocations = false;
    allocatedPointers.insert(ptr);
    monitoringAllocations = true;
  }

  return ptr;
}

inline void operator delete(void *ptr) {
  if (const auto it = allocatedPointers.find(ptr); it != allocatedPointers.end()) {
    allocatedPointers.erase(it);
  }

  std::free(ptr);
}

inline void operator delete(void *ptr, std::size_t) {
  operator delete(ptr);
}

inline void operator delete[](void *ptr) {
  if (const auto it = allocatedPointers.find(ptr); it != allocatedPointers.end()) {
    allocatedPointers.erase(it);
  }

  std::free(ptr);
}

inline void operator delete[](void *ptr, std::size_t) {
  operator delete[](ptr);
}

template<std::integral N>
consteval N TestUtility::exp2(N num) {
  size_t value = 2;

  --num;

  while (num > 0) {
    value *= 2;
    --num;
  }

  return value;
}

template<std::integral T>
RandomNumberGenerator<T>::RandomNumberGenerator() : engine{std::random_device{}()} {
}

template<std::integral T>
T RandomNumberGenerator<T>::operator()(const T min, const T max) {
  return std::uniform_int_distribution<T>{min, max}(engine);
}

inline TestError::TestError(std::string &&message) : message{std::move(message)} {
}

inline TestError::TestError(const std::string &message) : message{message} {
}

inline const char * TestError::what() const noexcept {
  return message.c_str();
}

template<string_compatible T>
std::string TestUtility::makeString(T object) {
  if constexpr (string_initializable<T>) {
    return std::string{object};
  }

  if constexpr (string_convertible<T>) {
    return static_cast<std::string>(object);
  }

  if constexpr (to_stringable<T>) {
    return std::to_string(object);
  }

  if constexpr (insertable<T>) {
    std::ostringstream oss;
    oss << object;
    return oss.str();
  }

  return "none";
}

template<typename... Args>
std::string TestUtility::concatenate(Args &&...args) {
  return (makeString(std::forward<Args>(args)) + ...);
}

template<typename... Args>
std::string TestUtility::NoMonitor_concatenate(Args &&...args) {
  if (monitoringAllocations) {
    monitoringAllocations = false;
    std::string s = concatenate(std::forward<Args>(args)...);
    monitoringAllocations = true;
    return s;
  }

  return concatenate(std::forward<Args>(args)...);
}

inline TestData::TestData(const char *str) : source{str}, nullSource{false} {
}

inline TestData::TestData(std::string &&str) : source{std::move(str)}, nullSource{false} {
}

inline void TestData::setSource(const char *str) {
  source = str;
  nullSource = false;
}

inline void TestData::setSource(std::string &&str) {
  source = std::move(str);
}

inline void TestData::setTask(const char *str) {
  task = str;
  nullTask = false;
}

inline void TestData::setTask(std::string &&str) {
  task = std::move(str);
  nullTask = false;
}

inline const std::string &TestData::getTest() const {
  return test;
}

inline const std::string &TestData::getSource() const {
  return source;
}

inline const std::string &TestData::getTask() const {
  return task;
}

inline void TestData::newTest(const std::string &str) {
  test = str;
  nullSource = true;
  nullTask = true;
}

inline bool TestData::sourceIsNull() const {
  return nullSource;
}

inline bool TestData::taskIsNull() const {
  return nullTask;
}

template<
  template <typename, typename...> typename Functor,
  template <typename, size_t> typename ChunkedListType,
  size_t ChunkSize = 1,
  size_t FinalChunkSize = 16,
  typename... Args
>
void TestUtility::callFunction(const char *functionName) {
  ++testNumber;
  std::cout << "Test " << testNumber << ": " << functionName << '\n';

  try {
    testData.newTest(functionName);
    allocatedPointers.clear();

    monitoringAllocations = true;
    Tests::Test<ChunkedListType, Functor>{}.template call<ChunkSize, FinalChunkSize, Args...>();
    monitoringAllocations = false;

    if (!allocatedPointers.empty()) {
      std::ostringstream undeletedMemory;
      undeletedMemory << "Memory has not been deleted:\n";

      for (const auto ptr : allocatedPointers) {
        undeletedMemory << ptr << '\n';
      }

      throw TestError(undeletedMemory.str());
    }
  } catch (const TestError &e) {
    throw e;
  } catch (const std::exception &e) {
    throw std::runtime_error(
      concatenate("Call to ", functionName, " failed\nSource: ",
            testData.sourceIsNull() ? "NULL" : testData.getSource(),
            "\nTask: ", testData.taskIsNull() ? "NULL" : testData.getTask(),
            "\nError: ", e.what())
    );
  }

  std::cout << "Test " << testNumber << " successful\n" << std::endl;
}

inline void TestUtility::performTask(const char *taskName, const int logLevel) {
  monitoringAllocations = false;

  testData.setTask(taskName);
  if (logLevel <= LogLevel) {
    std::cout << taskName << '\n';
  }

  monitoringAllocations = true;
}

template<std::integral Number>
std::string TestUtility::ordinalize(Number n) {
  switch (std::string &&str = std::to_string(n); str.back()) {
    case '1':
      str += "st";
      return str;
    case '2':
      str += "nd";
      return str;
    case '3':
      str += "rd";
      return str;
    default:
      str += "th";
      return str;
  }
}

template<template<typename, size_t> typename ChunkedListType, template<typename, typename...> typename Functor>
template<size_t ChunkSize, size_t FinalChunkSize, typename... Args>
void Tests::Test<ChunkedListType, Functor>::secondaryCall(const size_t testNumber) const {
  testData.setSource(std::move(concatenate("Test ", testNumber)));
  Functor<ChunkedListType<DefaultT, ChunkSize>, Args...>{}();

  if constexpr (FinalChunkSize > ChunkSize) {
    secondaryCall<ChunkSize + 1, FinalChunkSize, Args...>(testNumber + 1);
  }
}

template<template<typename, size_t> typename ChunkedListType, template<typename, typename...> typename Functor>
template<size_t ChunkSize, size_t FinalChunkSize, typename... Args>
  requires(FinalChunkSize >= ChunkSize)
void Tests::Test<ChunkedListType, Functor>::call() const {

  testData.setSource("Test 1");
  Functor<ChunkedListType<DefaultT, ChunkSize>, Args...>{}();

  if constexpr (FinalChunkSize > ChunkSize) {
    secondaryCall<ChunkSize + 1, FinalChunkSize, Args...>(2);
  }
}

template<typename ChunkedListType>
void Tests::Initialization<ChunkedListType>::operator()() const {
  using AccessorType = Accessor<ChunkedListType>;

  {
    performTask("Constructing list with default initializer");
    ChunkedListType list;

    performTask("Reinterpreting the memory address of a ChunkedList as a ChunkedListAccessor pointer");
    AccessorType *accessor = reinterpret_cast<AccessorType *>(&list);

    performTask("Calling accessor methods");
    THROW_IF(!accessor->front, "Expected ChunkedList to have front")
    THROW_IF(accessor->front != accessor->back, "Expected front and back to be equal in empty ChunkedList")
  }
  {
    performTask("Constructing list with initializer list");
    ChunkedListType list{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    AccessorType *accessor = reinterpret_cast<AccessorType *>(&list);
    THROW_IF(!accessor->back, "Expected ChunkedList to have back")
    THROW_IF(!accessor->front, "Expected ChunkedList to have front")
  }
}

template<typename ChunkedListType>
void Tests::Chunk<ChunkedListType>::operator()() const {
  using AccessorType = Accessor<ChunkedListType>;
  using ChunkType = typename AccessorType::Chunk;

  constexpr size_t CHUNK_SIZE = ChunkedListType::chunk_size;
  constexpr size_t ENLARGED_CHUNK_SIZE = CHUNK_SIZE * 2;
  using LargeChunkType = typename ChunkedListAccessor<DefaultT, ENLARGED_CHUNK_SIZE>::Chunk;

  indexing<LargeChunkType>();
  iteration<ChunkType>();
  advancing<ChunkType>();
}

template<typename ChunkedListType>
template<typename LCT>
void Tests::Chunk<ChunkedListType>::indexing() {
  performTask("Chunk indexing");
  std::vector<DefaultT> vec;

  for (size_t i = 0; i < LCT::chunk_size; ++i) {
    vec.push_back(static_cast<DefaultT>(i));
  }

  std::unique_ptr<LCT> chunk = std::make_unique<LCT>(vec.data(), vec.size());

  for (size_t i = 0; i < LCT::chunk_size; ++i) {
    THROW_IF((*chunk)[i] != static_cast<DefaultT>(i), concatenate("Expected ", ordinalize(i), " element of chunked list to be equal to ", i))
  }

}

template<typename ChunkedListType>
template<typename CT>
void Tests::Chunk<ChunkedListType>::iteration() {
  performTask("Chunk iteration");
  RandomNumberGenerator<DefaultT> rng;

  constexpr size_t CHUNK_SIZE = CT::chunk_size;

  std::vector<DefaultT> vec;
  vec.reserve(CHUNK_SIZE);

  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    vec.push_back(static_cast<DefaultT>(i));
  }

  std::unique_ptr<CT> chunkPtr = std::make_unique<CT>(vec.data(), CHUNK_SIZE);

  DefaultT expected = 0;
  for (auto it = chunkPtr->begin(); it != chunkPtr->end(); ++it) {
    auto item = *it;
    THROW_IF(item != expected, concatenate("Expected item to be equal to ", expected, " but received ", item))
    ++expected;
  }

  THROW_IF(static_cast<size_t>(expected) != CHUNK_SIZE, concatenate("Expected 'expected' to finish at ", CHUNK_SIZE, " but received ", expected))
}

template<typename ChunkedListType>
template<typename CT>
void Tests::Chunk<ChunkedListType>::advancing() {
  performTask("Chunk advancing");

  CT *front = new CT{};
  CT *back = front;

  constexpr size_t RANGE = 3;

  // Allocation

  for (size_t i = 0; i < RANGE; ++i) {
    CT *newBack = new CT{nullptr, 0, nullptr, back};
    back->nextChunk = newBack;
    back = newBack;
  }

  CT *expectedBack = &((*front) + RANGE);

  THROW_IF(expectedBack != back, concatenate("Expected front + ", RANGE, " to be equal to back"))

  CT *expectedFront = &((*back) - RANGE);

  THROW_IF(expectedFront != front, concatenate("Expected front + ", RANGE, " to be equal to front"))

  // Deallocation

  do {
    CT *newFront = front->nextChunk;
    delete front;
    front = newFront;
  } while (front); // repeats RANGE + 1 times
}

template<typename ChunkedListType>
template<typename CT>
void Tests::Chunk<ChunkedListType>::insertion() {
  performTask("Chunk fields");

  constexpr size_t CHUNK_SIZE = CT::chunk_size;

  CT chunk;

  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    chunk.push(DefaultT{1});
  }

  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    const DefaultT item = chunk[i];
    THROW_IF(static_cast<size_t>(item) != i, concatenate("Expected item to be equal to ", i))
  }
}

template<typename ChunkedListType>
void Tests::ListEnds<ChunkedListType>::operator()() {
  performTask("Creating chunked list with initializer list");

  using AccessorType = Accessor<ChunkedListType>;

  ChunkedListType chunkedList{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  AccessorType *accessor = reinterpret_cast<AccessorType*>(&chunkedList);

  auto front = accessor->front;
  auto back = accessor->back;

  THROW_IF((*front)[0] != 1, "First item is not 1")
  THROW_IF((*back)[back->size() - 1] != 10, "Last item is not 10")

  performTask("Calling size()");
  const size_t size = chunkedList.size();

  THROW_IF(size != 10, concatenate("Unexpected size; received ", size, " but expected 10"));

  performTask("Indexing");
  THROW_IF(chunkedList[size - 1] != 10, "Last item is not 10")
}

template<typename ChunkedListType>
void Tests::Insertion<ChunkedListType>::operator()() {
  ChunkedListType chunkedList{5, 10, 15};
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

  THROW_IF(str != expectedOutput, concatenate("ostream insertion ran incorrectly\nReceived: ", str, "\nExpected: ", expectedOutput))
}

template<typename ChunkedListType>
void Tests::Sorting<ChunkedListType>::operator()() const {
  const std::string initialSource = testData.getSource();

  for (int i = 0; i < 10; ++i) {
#define EXTEND_SOURCE(extension) testData.setSource(std::move(NoMonitor_concatenate(initialSource, extension)));

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
}

template<typename ChunkedListType>
template<SortType SortingAlgorithm>
void Tests::Sorting<ChunkedListType>::sort() const {
  performTask("List creation");
  ChunkedListType list;

  performTask("Random Number Generator creation");
  RandomNumberGenerator rng;

  performTask("Pushing or using the RNG");
  for (int i{}; i < 100; ++i) {
    list.push(rng(-1'000'000'000, 1'000'000'000));
  }

  performTask("Sorting");
  list.template sort<std::less<DefaultT>, SortingAlgorithm>();

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

template<typename ChunkedListType>
void Tests::PushingAndPopping<ChunkedListType>::operator()() const {
  ChunkedListType chunkedList{};

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
  THROW_IF(firstItem != 'a', "First item is not 'a'")
}

template<typename ChunkedListType>
void Tests::SlicesAndIterators<ChunkedListType>::operator()() const {
  performTask("List creation");
  ChunkedListType chunkedList;

  constexpr size_t MAX = 10;

  performTask("Pushing");
  for (size_t i = 1; i <= MAX; ++i) {
    chunkedList.push(static_cast<DefaultT>(i));
  }

  const ChunkedListType &constListRef = chunkedList;

  {
    performTask("Slicing (1)");
    RandomNumberGenerator rng;

    const size_t startOffset = rng(0, MAX - 2);
    const size_t endOffset = rng(startOffset, MAX - 1);

    auto slice = chunkedList.VARIANT_CODE(slice, get_slice)
      (begin(chunkedList) + startOffset, begin(chunkedList) + endOffset);

    auto slice2 = constListRef.VARIANT_CODE(slice, get_slice)
      (begin(chunkedList) + startOffset, begin(chunkedList) + endOffset);

    THROW_IF(slice != slice2, "");

    for (size_t index = 0; index < endOffset - startOffset; ++index) {
      const DefaultT expected = startOffset + index + 1;
      THROW_IF(
        slice[index] != expected,
        (ordinalize(index) += " item in slice is not equal to ") += std::to_string(expected)
      );
    }

    DefaultT expected = startOffset + 1;
    for (const int num : slice) {
      THROW_IF(num != expected, concatenate("Expected ", expected, " but received ", num))
      ++expected;
    }

    const DefaultT expectedExpected = endOffset + 1;
    THROW_IF(
      expected != expectedExpected,
      concatenate("Expected 'expected' to be equal to ", expectedExpected, " but received ", expected)
    );
  }
  {
    performTask("Slicing (2)");
    RandomNumberGenerator<size_t> rng;
    const size_t startIndex = rng(0, MAX - 2);
    const size_t endIndex = rng(startIndex, MAX - 1);

    DefaultT expected = startIndex + 1;

    for (auto slice = chunkedList.VARIANT_CODE(slice, get_slice)(startIndex, endIndex); auto item : slice) {
      THROW_IF(
        item != expected,
        concatenate("Expected item to be equal to ", expected, " but received ", item)
      );
      ++expected;
    }

    const DefaultT expectedExpected = endIndex + 1;
    THROW_IF(
      expected != expectedExpected,
      concatenate("Expected 'expected' to be equal to ", expectedExpected, " but got ", expected)
    )
  }
  {
    int total{};

    performTask("List iteration (implicit)");
    for (const int num: chunkedList)
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
    THROW_IF(begin + OFFSET != constBegin + OFFSET, concatenate("Iterator + ", OFFSET, " is unequal to const Iterator + ", OFFSET));
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
        DefaultT data = chunkRef[i];
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

template<typename ChunkedListType>
void Tests::ConcatenationAndIndexing<ChunkedListType>::operator()() const {
  ChunkedListType chunkedList;

  performTask("Concatenating"); {
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
    THROW_IF(
      str != "0 1 2 3 4 5 6 7 8 9",
      concatenate("Concatenation failed; expected \"0 1 2 3 4 5 6 7 8 9\" but received \"", str, '"')
    )
  }

  performTask("Indexing");
  for (int i = 0; i < 10; ++i) {
    const int num = chunkedList[i];
    THROW_IF(num != i, concatenate("Index ", i, " was expected to be ", i, " but received ", num));
  }
}

template<typename ChunkedListType>
void Tests::EqualityAndInequality<ChunkedListType>::operator()() const {
  ChunkedListType list1{};
  ChunkedListType list2{};

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
