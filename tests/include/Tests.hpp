#pragma once

#include "test_utility.hpp"

constexpr char BoundaryChar = '=';
constexpr size_t BoundaryCharacters = 80;

inline std::string calculateBoundary() {
  std::string boundary;
  boundary.reserve(BoundaryCharacters);

  for (size_t i = 0; i < BoundaryCharacters; ++i) {
    boundary.push_back(BoundaryChar);
  }

  return boundary;
}

inline std::string &getBoundary() {
  static std::string boundary = calculateBoundary();
  return boundary;
}

#define DISPLAY_BOUNDARY std::cout << getBoundary() << '\n';

#define ASSERT(expr)                                                                                                   \
  if (!(expr)) {                                                                                                       \
    using namespace test_utility;                                                                                      \
    throw TestError{concatenate("Assertion ", #expr, " on line ", __LINE__, " failed!")};                              \
  }

#define ASSERT_OBJ(constructionExpression, ...)                                                                        \
  do {                                                                                                                 \
    auto obj = constructionExpression;                                                                                 \
    test_utility::variadicAssert(__LINE__, #__VA_ARGS__, __VA_ARGS__);                                                 \
  } while (false);

#define ASSERT_INCREMENT(firstIt, lastIt, ...)                                                                         \
  std::for_each(firstIt, lastIt, [&counter](__VA_ARGS__ Integral &n) {                                                 \
    ASSERT(n == counter)                                                                                               \
    ++counter;                                                                                                         \
  });

#define DECREMENT_ASSERT(firstIt, lastIt, ...)                                                                         \
  std::for_each(firstIt, lastIt, [&counter](__VA_ARGS__ Integral &n) {                                                 \
    --counter;                                                                                                         \
    ASSERT(n == counter)                                                                                               \
  });

#define TIME_PROCESS(process, ...)                                                                                     \
  do {                                                                                                                 \
    namespace chrono = std::chrono;                                                                                    \
    const auto start = chrono::high_resolution_clock::now();                                                           \
    {                                                                                                                  \
      process;                                                                                                         \
    };                                                                                                                 \
    const auto end = chrono::high_resolution_clock::now();                                                             \
    print(__VA_ARGS__);                                                                                                \
    outputDuration(end - start);                                                                                       \
  } while (false);

#define TIME_REGISTERED_TESTS                                                                                          \
  do {                                                                                                                 \
    using namespace test_utility;                                                                                      \
    using Tests::Registry::getRegistry;                                                                                \
    TIME_PROCESS(                                                                                                      \
      for (auto test : getRegistry()) test();                                                                          \
      , concatenate(getBoundary(), "\nAll ") +=                                                                        \
        (testNumber > 1 ? concatenate(testNumber, " tests have completed in ").c_str() : "tests have completed in ")); \
    std::cout << ".\n" << std::endl;                                                                                   \
  } while (false);

#define TEST_MAIN(testName, testType)                                                                                  \
  int main() {                                                                                                         \
    std::cout << "Starting " << #testName << ' ' << #testType << " test" << std::endl;                                 \
    DISPLAY_BOUNDARY                                                                                                   \
    std::cout << std::endl;                                                                                            \
    TIME_REGISTERED_TESTS                                                                                              \
    return EXIT_SUCCESS;                                                                                               \
  }

#define INTEGRATION_TEST(name) TEST_MAIN(name, integration)

#define UNIT_TEST(name) TEST_MAIN(name, unit)

#define TEST_DEFS

#define ALIGNED_ARRAY_DEF                                                                                              \
  template<typename T, size_t N = 1>                                                                                   \
  using AlignedArray = test_utility::AlignedArray<T, N>;

#define CHUNKED_LIST_DEFS                                                                                              \
  using PubList = chunked_list::Accessor<List>;                                                                        \
  using AlignedList = test_utility::AlignedArray<List>;                                                                \
  using IntegralInitializerList = std::initializer_list<Integral>;                                                     \
                                                                                                                       \
  PubList &access(List &list) const { return *reinterpret_cast<PubList *>(&list); }                                    \
                                                                                                                       \
  const PubList &access(const List &list) const { return *reinterpret_cast<PubList *>(&list); }

#define CHUNKED_LIST_ITERATOR_DEFS                                                                                     \
  using _iterator = typename List::iterator;                                                                           \
  using _const_iterator = typename List::const_iterator;                                                               \
  using _reverse_iterator = typename List::reverse_iterator;                                                           \
  using _const_reverse_iterator = typename List::const_reverse_iterator;                                               \
  using _chunk_iterator = typename List::chunk_iterator;                                                               \
  using _const_chunk_iterator = typename List::const_chunk_iterator;                                                   \
  using _reverse_chunk_iterator = typename List::reverse_chunk_iterator;                                               \
  using _const_reverse_chunk_iterator = typename List::const_reverse_chunk_iterator;

#define CHUNKED_LIST_SLICE_DEFS                                                                                        \
  using MutableSlice = typename List::mutable_slice;                                                                   \
  using ImmutableSlice = typename List::immutable_slice;

#define CHUNK_DEFS                                                                                                     \
  using ChunkAllocator = typename List::template allocator_type<Chunk>;                                                \
  using ChunkAllocatorTraits = std::allocator_traits<ChunkAllocator>;                                                  \
                                                                                                                       \
  ChunkAllocator chunk_allocator;                                                                                      \
                                                                                                                       \
  template<template<typename> typename Allocator>                                                                      \
  using rebind_chunk_alloc = typename CurrentList<Integral, ChunkSize, Allocator>::Chunk;

#define SUBTEST(testName)                                                                                              \
  namespace Tests {                                                                                                    \
    template<size_t ChunkSize>                                                                                         \
    class testName final : BaseTest {                                                                                  \
        template<typename T>                                                                                           \
        using Allocator = test_utility::CustomAllocator<T>;                                                            \
        using List = CurrentList<Integral, ChunkSize, Allocator>;                                                      \
        using Chunk = typename List::Chunk;                                                                            \
                                                                                                                       \
        TEST_DEFS                                                                                                      \
      public:                                                                                                          \
        static constexpr const char *name = #testName;                                                                 \
        void operator()() override;                                                                                    \
    };                                                                                                                 \
                                                                                                                       \
    namespace Registry {                                                                                               \
      static Registrar testName##Registry{test_utility::callSubTest<Tests::testName>};                                 \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  template<size_t ChunkSize>                                                                                           \
  void Tests::testName<ChunkSize>::operator()()

#define PERFORMANCE_BENCHMARK(name)                                                                                    \
  namespace Tests {                                                                                                    \
    void main();                                                                                                       \
  }                                                                                                                    \
                                                                                                                       \
  int main() {                                                                                                         \
    std::cout << "Starting " << name << " performance benchmark\n";                                                    \
                                                                                                                       \
    DISPLAY_BOUNDARY                                                                                                   \
    std::cout << std::endl;                                                                                            \
    Tests::main();                                                                                                     \
    DISPLAY_BOUNDARY                                                                                                   \
    test_utility::assertLeakage();                                                                                     \
  }                                                                                                                    \
                                                                                                                       \
  void Tests::main()

namespace Tests {
  namespace Registry {
    using FunctionType = void (*)();

    inline std::vector<FunctionType> &getRegistry() {
      static std::vector<FunctionType> registry;
      return registry;
    }

    class Registrar {
      public:
        Registrar(FunctionType function) { getRegistry().emplace_back(function); }
    };
  } // namespace Registry

  namespace Types {
    template<std::integral T = int, template<typename> typename TemplateAllocator = test_utility::CustomAllocator>
    struct Integral {
        using Allocator = TemplateAllocator<T>;

        using AllocatorTraits = std::allocator_traits<Allocator>;

        Allocator allocator;

        int *num;

        Integral() : num{AllocatorTraits::allocate(allocator, 1)} {}

        explicit Integral(const int _num) : Integral{} { new (num) int{_num}; }

        explicit Integral(const size_t _num) : Integral{} { new (num) int{static_cast<T>(_num)}; }

        Integral(const Integral &other) : Integral(*other.num) {}

        ~Integral() { AllocatorTraits::deallocate(allocator, num, 1); }

        template<std::integral U>
        constexpr Integral &operator=(const U other) {
          *num = other;
          return *this;
        }

        constexpr Integral &operator=(Integral &&other) noexcept {
          AllocatorTraits::deallocate(allocator, num, 1);
          num = other.num;
          return *this;
        }

        constexpr Integral &operator=(const Integral &other) {
          if (this != &other) {
            *num = *other.num;
          }
          return *this;
        }

        template<std::integral U>
        Integral operator+(const U other) const {
          return Integral{*num + other};
        }

        Integral operator+(const Integral &other) const { return Integral{*num + *other.num}; }

        template<std::integral U>
        Integral &operator+=(const U other) {
          *num += other;
          return *this;
        }

        Integral &operator+=(const Integral &other) {
          *num += *other.num;
          return *this;
        }

        template<std::integral U>
        Integral operator-(const U other) const {
          return Integral{*num - other};
        }

        Integral operator-(const Integral &other) const { return Integral{*num + *other.num}; }

        Integral &operator++() {
          ++*num;
          return *this;
        }

        Integral operator++(int) { // NOLINT
          Integral original = *this;
          ++*num;
          return original;
        }

        Integral &operator--() {
          --*num;
          return *this;
        }

        Integral operator--(int) { // NOLINT
          Integral original = *this;
          --*num;
          return original;
        }

        template<std::integral U>
        bool operator==(const U other) const {
          return *num == static_cast<T>(other);
        }

        bool operator==(const Integral &other) const { return *num == *other.num; }

        template<std::integral U>
        bool operator!=(const U other) const {
          return *num == static_cast<T>(other);
        }

        bool operator!=(const Integral &other) const { return *num != *other.num; }

        operator int() const { return *num; }
    };

    using String = std::basic_string<char, std::char_traits<char>, test_utility::CustomAllocator<char>>;
  } // namespace Types

  class BaseTest {
    public:
      using Integral = Types::Integral<>;
      using Vector = std::vector<Integral>;

      template<typename T>
      using Allocator = test_utility::CustomAllocator<T>;

      virtual void operator()() = 0;

      virtual ~BaseTest() = default;
  };
} // namespace Tests
