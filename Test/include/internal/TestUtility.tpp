#pragma once

#include "TestUtility.hpp"
#include "chunked-list/ChunkedListAccessor.hpp"

#include <chrono>
#include <cstdlib>
#include <list>
#include <memory>
#include <new>
#include <ranges>

using namespace TestUtility;

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
RandomNumberGenerator<T>::RandomNumberGenerator() : engine{std::random_device{}()} {}

template<std::integral T>
T RandomNumberGenerator<T>::operator()(const T min, const T max) {
  return std::uniform_int_distribution<T>{min, max}(engine);
}

inline TestError::TestError(std::string &&message) : message{std::move(message)} {}

inline TestError::TestError(const std::string &message) : message{message} {}

inline const char *TestError::what() const noexcept { return message.c_str(); }

inline TestData::TestData(const char *str) : source{str}, nullSource{false} {}

inline TestData::TestData(std::string &&str) : source{std::move(str)}, nullSource{false} {}

inline void TestData::setSource(const char *str) {
  source = str;
  nullSource = false;
}

inline void TestData::setSource(std::string &&str) { source = std::move(str); }

inline void TestData::setTask(const char *str) {
  task = str;
  nullTask = false;
}

inline void TestData::setTask(std::string &&str) {
  task = std::move(str);
  nullTask = false;
}

inline const std::string &TestData::getTest() const { return test; }

inline const std::string &TestData::getSource() const { return source; }

inline const std::string &TestData::getTask() const { return task; }

inline void TestData::newTest(const std::string &str) {
  test = str;
  nullSource = true;
  nullTask = true;
}

inline bool TestData::sourceIsNull() const { return nullSource; }

inline bool TestData::taskIsNull() const { return nullTask; }

template<template<typename> typename Functor, template<typename, size_t, typename> typename ChunkedListType,
         size_t ChunkSize = 1, size_t FinalChunkSize = 16>
void TestUtility::callFunction() {
  using TestCaller = Tests::Test<ChunkedListType, Functor>;
  const char *testName = TestCaller{}.name;

  ++testNumber;
  std::cout << "Test " << testNumber << ": " << testName << '\n';

  namespace chrono = std::chrono;

  using chrono::high_resolution_clock;
  using chrono::time_point;

  time_point<high_resolution_clock> start, end;

  try {
    testData.newTest(testName);

    start = high_resolution_clock::now();
    TestCaller{}.template call<ChunkSize, FinalChunkSize>();
    end = high_resolution_clock::now();
  } catch (const std::exception &e) {
    throw std::runtime_error(
      concatenate("Call to ", testName, " failed\nSource: ", testData.sourceIsNull() ? "NULL" : testData.getSource(),
                  "\nTask: ", testData.taskIsNull() ? "NULL" : testData.getTask(), "\nError: ", e.what()));
  }

  if (!allocatedSet.empty()) {
    std::ostringstream leakedMemoryAddresses;

    for (const auto address : allocatedSet) {
      leakedMemoryAddresses << address << '\n';
    }

    throw TestError{concatenate("Memory leaked after calling ", testName, ":\n", leakedMemoryAddresses)};
  }

  std::cout << "Test " << testNumber << " successful (";

  using chrono::duration_cast;

  const auto executionTime = end - start;

  if (const auto msDuration = duration_cast<chrono::milliseconds>(executionTime); msDuration.count() < 10) {
    if (const auto usDuration = duration_cast<chrono::microseconds>(executionTime); usDuration.count() < 10) {
      std::cout << duration_cast<chrono::nanoseconds>(executionTime);
    } else {
      std::cout << usDuration;
    }
  } else {
    std::cout << msDuration;
  }

  std::cout << ")\n" << std::endl;
}

inline void TestUtility::performTask(const char *taskName, const int logLevel) {
  testData.setTask(taskName);
  if (logLevel <= LogLevel) {
    std::cout << taskName << '\n';
  }
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

template<template<typename, size_t, typename> typename ChunkedListType, template<typename> typename Functor>
template<size_t ChunkSize, size_t FinalChunkSize>
void Tests::Test<ChunkedListType, Functor>::secondaryCall(const size_t testNumber) const {
  testData.setSource(concatenate("Test ", testNumber));
  Functor<ChunkedListType<IntegralT, ChunkSize, CustomAllocator<IntegralT>>>{}();

  if constexpr (FinalChunkSize > ChunkSize) {
    secondaryCall<ChunkSize + 1, FinalChunkSize>(testNumber + 1);
  }
}

template<template<typename, size_t, typename> typename ChunkedListType, template<typename> typename Functor>
template<size_t ChunkSize, size_t FinalChunkSize>
  requires(FinalChunkSize >= ChunkSize)
void Tests::Test<ChunkedListType, Functor>::call() const {
  testData.setSource("Test 1");
  Functor<ChunkedListType<IntegralT, ChunkSize, CustomAllocator<IntegralT>>>{}();

  if constexpr (FinalChunkSize > ChunkSize) {
    secondaryCall<ChunkSize + 1, FinalChunkSize>(2);
  }
}
