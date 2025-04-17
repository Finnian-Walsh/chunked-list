#pragma once

#include <chrono>
#include <iostream>
#include <new>
#include <random>
#include <unordered_set>

#include "chunked_list/detail/utility.hpp"

namespace test_utility {
  namespace chunked_list_utility = chunked_list::utility;

  using chunked_list_utility::can_construct;
  using chunked_list_utility::concatenate;

  inline int testNumber{0};

  inline std::unordered_set<void *> allocatedSet{};

  template<typename T, bool O = false>
  class MallocAllocator {
    protected:
      static constexpr bool Output = O;

    public:
      using value_type = T;

      MallocAllocator() = default;

      template<typename U>
      explicit MallocAllocator(const MallocAllocator<U> &);

      ~MallocAllocator() = default;

      static T *allocate(std::size_t n);

      static void deallocate(T *ptr, std::size_t);

      template<typename... Args>
      static void construct(T *ptr, Args &&...args);

      static void destroy(T *ptr);
  };

  template<typename T>
  class CustomAllocator : public MallocAllocator<T> {};

  template<typename T>
  class CustomLogAllocator : public MallocAllocator<T, true> {};

  template<typename T, size_t N = 1>
  class AlignedArray {
      using AlignedArrayType = std::aligned_storage_t<sizeof(T), alignof(T)>;
      alignas(T) std::byte array[N][sizeof(T)]{};

    public:
      AlignedArray() = default;

      T *operator+(size_t index);

      T *operator-(size_t index);

      T &operator*();

      T *operator->();

      T &operator[](size_t index);

      operator T *();

      T *data();

      template<typename... Args>
      void construct(size_t index, Args &&...args);

      void destroy(size_t index);
  };

  template<typename... Args>
  void print(Args... args);

  template<typename... Args>
  void outputDuration(std::chrono::duration<Args...> Duration);

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

  inline void assertLeakage();

  inline size_t contextEnd(const std::string &context);

  inline std::string nextContext(const std::string &context);

  template<typename... Args>
  void variadicAssert(int, const std::string &, Args...);

  template<typename First, typename... Args>
  void variadicAssert(int line, const std::string &context, First first, Args... args);

  template<typename A, typename B>
  bool equalContainersIdx(const A &a, const B &b);

  template<template<size_t> typename SubTest, size_t CurrentChunkSize = 1, size_t MaxChunkSize = 16>
  void chainCall();

  template<typename F, typename... Args>
  void timeFunction(F f, Args &&...message);

  template<template<size_t> typename SubTest>
  void callSubTest();

  template<typename F>
  void callPerformanceTest(F function, const char *name);

  template<std::integral Number>
  std::string ordinalize(Number n);
} // namespace test_utility

#include "detail/test_utility.tpp"
