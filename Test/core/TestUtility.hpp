#pragma once

#include <ChunkedList.hpp>
#include <unistd.h>
#include <random>

#define BEGIN std::cout << "Starting tests..." << std::endl;
#define SUCCESS std::cout << "All " << testNumber << " tests have been ran." << std::endl; return EXIT_SUCCESS;
#define THROW_IF(condition, str) if (condition) throw std::runtime_error(str);

#ifndef CHUNKED_LIST_TYPE
#define CHUNKED_LIST_TYPE 1
#endif

#if CHUNKED_LIST_TYPE == 1
#define VARIANT_CODE(CamelCaseChunkedListCode, SnakeCaseChunkedListCode) CamelCaseChunkedListCode
#elif CHUNKED_LIST_TYPE == 2
#define VARIANT_CODE(CamelCaseChunkedListCode, SnakeCaseChunkedListCode) SnakeCaseChunkedListCode
#else
#error "Unsupported chunked list variant"
#endif

namespace TestUtility {
#ifdef LOG_LEVEL
  constexpr inline int LogLevel = LOG_LEVEL;
#else
  constexpr inline int LogLevel = 0;
#endif

  inline int testNumber{0};

  class RandomNumberGenerator {
    std::mt19937 engine;

    public:
      RandomNumberGenerator();

      int operator()(int min, int max);
  };

  inline class TestData {
    std::string test{};
    std::string source{};
    std::string task{};

    bool nullSource{true};
    bool nullTask{true};

    public:
      TestData() = default;

      explicit TestData(const char *str);

      explicit TestData(std::string &&str);

      void setSource(const char *str);

      void setSource(std::string &&str);

      void setTask(const char *str);

      void setTask(std::string &&str);

      const std::string &getTest() const;

      const std::string &getSource() const;

      const std::string &getTask() const;

      void newTest(const std::string &str);

      bool sourceIsNull() const;

      bool taskIsNull() const;
  } testData;

  void callFunction(const char *functionName, void (*functionPtr)());

  void performTask(const char *taskName, int logLevel = 10);

  template<typename T, size_t ChunkSize>
  class ChunkedListAccessor final : ChunkedList<T, ChunkSize> {
    using DerivedChunkedList = ChunkedList<T, ChunkSize>;

    public:
      size_t getChunkCount() {
        return this->chunkCount;
      }

      typename DerivedChunkedList::Chunk *getFront() {
        return this->front;
      }

      typename DerivedChunkedList::Chunk *getBack() {
        return this->back;
      }
  };

  namespace Tests {
    using DefaultT = int;
    constexpr size_t DefaultChunkSize = 32;

    template<template <typename, size_t> typename ChunkedListType, size_t ChunkSize>
    void FrontAndBack();

    template<template <typename, size_t> typename ChunkedListType, size_t ChunkSize>
    void Insertion();

    template<template<typename, size_t> typename ChunkedListType, SortType SortingAlgorithm, size_t ChunkSize>
    void Sorting();

    template<template <typename, size_t> typename ChunkedListType, size_t ChunkSize>
    void Iterators();

    template<template <typename, size_t> typename ChunkedListType, size_t ChunkSize>
    void PushingAndPopping();

    template<template <typename, size_t> typename ChunkedListType, size_t ChunkSize>
    void EqualityAndInequality();

    template<template <typename, size_t> typename ChunkedListType, size_t ChunkSize>
    void ConcatenationAndIndexing();
  }
}

#include "TestUtility.tpp"
