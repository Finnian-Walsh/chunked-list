#pragma once

#include <random>
#include <type_traits>
#include <unistd.h>
#include <unordered_set>

#include "chunked-list/ChunkedList.hpp"
#include "chunked-list/ChunkedListAccessor.hpp"
#include "chunked-list/ChunkedListSnake.hpp"

using namespace chunked_list;
using namespace utility;

#define BEGIN std::cout << "Starting tests..." << std::endl;
#define SUCCESS                                                                                                        \
  std::cout << "All " << testNumber << " tests have been ran." << std::endl;                                           \
  return EXIT_SUCCESS;
#define THROW_IF(condition, str)                                                                                       \
  if (condition) {                                                                                                     \
    throw std::runtime_error(str);                                                                                     \
  }

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

      template<typename U>
      explicit MallocAllocator(const MallocAllocator<U> &) {}

      ~MallocAllocator() = default;

      static T *allocate(const std::size_t n) { return static_cast<T *>(malloc(n * sizeof(T))); }

      static void deallocate(T *ptr, const std::size_t) { free(ptr); }

      template<typename U, typename... Args>
      static void construct(U *ptr, Args &&...args) {
        new (ptr) U{std::forward<Args>(args)...};
      }

      template<typename U>
      static void destroy(U *ptr) {
        ptr->~U();
      }
  };

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

  template<template<typename> typename Functor, template<typename, size_t> typename ChunkedListType, size_t ChunkSize,
           size_t FinalChunkSize>
  void callFunction();

  void performTask(const char *taskName, int logLevel = 10);

  template<std::integral Number>
  std::string ordinalize(Number n);

  namespace Tests {
    using DefaultT = int;
    static_assert(std::is_integral_v<DefaultT>, "Default ChunkedList type must be an integral!");

    static constexpr size_t DefaultChunkSize = 16;
    static_assert(DefaultChunkSize > 0, "DefaultChunkSize must be greater than 0!");

    template<template<typename, size_t> typename ChunkedListType, template<typename> typename Functor>
    class Test {
        template<size_t ChunkSize, size_t FinalChunkSize>
        void secondaryCall(size_t testNumber) const;

      public:
        template<size_t ChunkSize, size_t FinalChunkSize>
          requires(FinalChunkSize >= ChunkSize)
        void call() const;

        const char *name = Functor<ChunkedListType<DefaultT, DefaultChunkSize>>{}.name;
    };

    template<typename>
    struct Chunk {
        const char *name = "Chunks";

        void operator()() const;

        template<typename LCT>
        static void initialization();

        template<typename LCT>
        static void indexing();

        template<typename CT>
        static void stacking();

        template<typename CT>
        static void arithmetic();

        template<typename CT>
        static void size();

        template<typename CT>
        static void comparison();

        template<typename CT>
        static void iteration();
    };

    template<typename>
    struct GenericChunkIterator {
        const char *name = "Generic Chunk Iterators";

        void operator()() const;
    };

    template<typename>
    struct Initialization {
        const char *name = "Initialization";

        void operator()() const;

        template<typename AT>
        static void default_construction();

        template<typename AT>
        static void initializer_list_construction();
    };

    template<typename>
    struct Edges {
        const char *name = "List edges";

        void operator()();
    };

    template<typename>
    struct Insertion {
        const char *name = "Insertion";

        void operator()();
    };

    template<typename>
    struct Sorting {
        const char *name = "Sorting";

        void operator()() const;

        template<SortType>
        void sort() const;
    };

    template<typename>
    struct SlicesAndIterators {
        const char *name = "Slices and iterators";

        void operator()() const;
    };

    template<typename>
    struct PushingAndPopping {
        const char *name = "Pushing and popping";

        void operator()() const;
    };

    template<typename>
    struct EqualityAndInequality {
        const char *name = "Equality and inequality";

        void operator()() const;
    };

    template<typename>
    struct ConcatenationAndIndexing {
        const char *name = "Concatenation and indexing";

        void operator()() const;
    };
  } // namespace Tests
} // namespace TestUtility

#include "internal/TestUtility.tpp"
#include "internal/Tests.tpp"
