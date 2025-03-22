#pragma once

#include <unistd.h>
#include <random>
#include <type_traits>
#include <unordered_set>

#include "chunked-list/ChunkedList.hpp"
#include "chunked-list/ChunkedListAccessor.hpp"
#include "chunked-list/ChunkedListSnake.hpp"

using namespace chunked_list;
using namespace utility;

#define BEGIN std::cout << "Starting tests..." << std::endl;
#define SUCCESS std::cout << "All " << testNumber << " tests have been ran without any memory leakage." << std::endl; return EXIT_SUCCESS;
#define THROW_IF(condition, str) if (condition) { throw std::runtime_error(str); }

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

  template<typename T>
  class MallocAllocator {
    public:
      using value_type = T;

      MallocAllocator() = default;

      template <typename U>
      explicit MallocAllocator(const MallocAllocator<U>&) {}

      ~MallocAllocator() = default;

      static T *allocate(const std::size_t n) {
        return static_cast<T *>(malloc(n * sizeof(T)));
      }

      static void deallocate(T *ptr, const std::size_t) {
        free(ptr);
      }

      template<typename U, typename... Args>
      static void construct(U *ptr, Args &&...args) {
        new (ptr) U{std::forward<Args>(args)...};
      }

      template<typename U>
      static void destroy(U *ptr) {
        ptr->~U();
      }
  };

  inline std::unordered_set<void *, std::hash<void*>, std::equal_to<void*>, MallocAllocator<void *>> allocatedPointers;

  inline bool monitoringAllocations;

#ifdef LOG_LEVEL
  constexpr inline int LogLevel = LOG_LEVEL;
#else
  constexpr inline int LogLevel = 0;
#endif

  inline int testNumber{0};

  template<std::integral N>
  consteval N exp2(N num);

  template<std::integral T = int>
  class RandomNumberGenerator {
    std::mt19937 engine;

    public:
      RandomNumberGenerator();

      T operator()(T min, T max);
  };

  class TestError final : public std::exception {
    std::string message;

    public:
      explicit TestError(std::string &&message);

      explicit TestError(const std::string &message);

       ~TestError() override = default;

      const char *what() const noexcept override;
  };

  template<typename T>
  concept string_initializable = requires(T obj) {
    { std::string{obj} };
  };

  template<typename T>
  concept string_convertible = std::is_convertible_v<T, std::string>;

  template<typename T>
  concept to_stringable = requires(T obj) {
    { std::to_string(obj) } -> std::same_as<std::string>;
  };

  template<typename T>
  concept insertable = requires(std::ostringstream oss, T obj) {
    { oss << obj };
  };

  template<typename T>
  concept string_compatible = string_initializable<T> || string_convertible<T> || to_stringable<T> || insertable<T>;

  template<string_compatible T>
  std::string makeString(T object);

  template<typename... Ts>
  std::string concatenate(Ts &&...args);

  template<typename... Ts>
  std::string NoMonitor_concatenate(Ts &&...args);

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

  template<
    template <typename, typename...> typename Functor,
    template <typename, size_t> typename ChunkedListType,
    size_t ChunkSize,
    size_t FinalChunkSize,
    typename... Args
  >
  void callFunction(const char *functionName);

  void performTask(const char *taskName, int logLevel = 10);

  template<std::integral Number>
  std::string ordinalize(Number n);

  namespace Tests {
    using DefaultT = int;
    static_assert(std::is_integral_v<DefaultT>, "Default ChunkedList type must be an integral!");

    static constexpr size_t DefaultChunkSize = 16;
    static_assert(DefaultChunkSize > 0, "DefaultChunkSize must be greater than 0!");

    template<template<typename, size_t> typename ChunkedListType, template<typename, typename...> typename Functor>
    class Test {
      template<
        size_t ChunkSize,
        size_t FinalChunkSize,
        typename... Args
      >
      void secondaryCall(size_t testNumber) const;

      public:
        template<size_t ChunkSize, size_t FinalChunkSize, typename... Args>
          requires(FinalChunkSize >= ChunkSize)
        void call() const;
    };

    template<typename>
    struct Initialization {
      void operator()() const;
    };

    template<typename>
    struct Chunk {
      void operator()() const;

      template<typename LCT>
      static void indexing();

      template<typename CT>
      static void iteration();

      template<typename CT>
      static void advancing();

      template<typename CT>
      static void insertion();
    };

    template<typename>
    struct ListEnds {
      void operator()();
    };

    template<typename>
    struct Insertion {
      void operator()();
    };

    template<typename>
    struct Sorting {
      void operator()() const;

      template<SortType>
      void sort() const;
    };

    template<typename>
    struct SlicesAndIterators {
      void operator()() const;
    };

    template<typename>
    struct PushingAndPopping {
      void operator()() const;
    };

    template<typename>
    struct EqualityAndInequality {
      void operator()() const;
    };

    template<typename>
    struct ConcatenationAndIndexing {
      void operator()() const;
    };
  }
}

#include "TestUtility.tpp"
