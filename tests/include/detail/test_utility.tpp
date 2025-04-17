#pragma once

#include "test_utility.hpp"

namespace test_utility {
  template<typename T, bool O>
  template<typename U>
  MallocAllocator<T, O>::MallocAllocator(const MallocAllocator<U> &) {}

  template<typename T, bool O>
  T *MallocAllocator<T, O>::allocate(const std::size_t n) {
    T *memory = static_cast<T *>(std::malloc(n * sizeof(T)));

    allocatedSet.insert(memory);

    if constexpr (Output) {
      std::cout << "Allocated memory " << memory << std::endl;
    }

    return memory;
  }

  template<typename T, bool O>
  void MallocAllocator<T, O>::deallocate(T *ptr, const std::size_t) {
    if (!allocatedSet.contains(ptr)) {
      throw std::runtime_error(concatenate("Double free or allocation mismatch: ", ptr, "!"));
    }

    if constexpr (Output) {
      std::cout << "Freeing " << ptr << std::endl;
    }

    allocatedSet.erase(ptr);

    std::free(ptr);
  }

  template<typename T, bool O>
  template<typename... Args>
  void MallocAllocator<T, O>::construct(T *ptr, Args &&...args) {
    new (ptr) T(std::forward<Args>(args)...);
  }

  template<typename T, bool O>
  void MallocAllocator<T, O>::destroy(T *ptr) {
    ptr->~T();
  }

  template<typename T, size_t N>
  T *AlignedArray<T, N>::load() {
    return std::launder(reinterpret_cast<T *>(array));
  }

  template<typename T, size_t N>
  const T *AlignedArray<T, N>::load() const {
    return std::launder(reinterpret_cast<const T *>(array));
  }

  template<typename T, size_t N>
  T *AlignedArray<T, N>::operator+(const size_t n) {
    return load() + n;
  }

  template<typename T, size_t N>
  const T *AlignedArray<T, N>::operator+(const size_t n) const {
    return load() + n;
  }

  template<typename T, size_t N>
  T *AlignedArray<T, N>::operator-(const size_t n) {
    return load() - n;
  }

  template<typename T, size_t N>
  const T *AlignedArray<T, N>::operator-(const size_t n) const {
    return load() - n;
  }

  template<typename T, size_t N>
  T &AlignedArray<T, N>::operator[](const size_t index) {
    return load()[index];
  }

  template<typename T, size_t N>
  const T &AlignedArray<T, N>::operator[](const size_t index) const {
    return load()[index];
  }

  template<typename T, size_t N>
  T &AlignedArray<T, N>::operator*() {
    return *load();
  }

  template<typename T, size_t N>
  const T &AlignedArray<T, N>::operator*() const {
    return *load();
  }

  template<typename T, size_t N>
  template<typename... Args>
  void AlignedArray<T, N>::construct(const size_t index, Args &&...args) {
    new (load() + index) T(std::forward<Args>(args)...);
  }

  template<typename T, size_t N>
  void AlignedArray<T, N>::destroy(const size_t index) {
    load()[index].~T();
  }

  template<typename... Args>
  void print(Args... args) {
    if constexpr (sizeof...(Args) > 0) {
      (std::cout << ... << std::forward<Args>(args)) << std::flush;
    }
  }

  template<typename... Args>
  void outputDuration(std::chrono::duration<Args...> Duration) {
    namespace chrono = std::chrono;
    using chrono::duration;

    if (const double duration_s = chrono::duration_cast<duration<double>>(Duration).count(); duration_s < 1.) {
      if (const double duration_ms = chrono::duration_cast<duration<double, std::milli>>(Duration).count();
          duration_ms < 1.) {
        if (const double duration_us = chrono::duration_cast<duration<double, std::micro>>(Duration).count();
            duration_us < 1.) {
          const double duration_ns = chrono::duration_cast<duration<double, std::nano>>(Duration).count();
          print(duration_ns, "ns");
        } else {
          print(duration_us, "us");
        }
      } else {
        print(duration_ms, "ms");
      }
    } else {
      print(duration_s, "s");
    }
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

  inline void assertLeakage() {
    if (!allocatedSet.empty()) {
      std::ostringstream memoryAddresses;
      const auto endIt = allocatedSet.end();

      for (auto it = allocatedSet.begin();; ++it) {
        constexpr size_t ADDRESSES_PER_LINE = 12;

        for (size_t i = 0; i < ADDRESSES_PER_LINE && it != endIt; ++i) {
          memoryAddresses << *it << '\t';
          ++it;
        }
        memoryAddresses << '\n';

        if (it == endIt) {
          break;
        }
      }

      throw TestError{concatenate("Memory leakage:\n", memoryAddresses)};
    }
  }

  inline size_t contextEnd(const std::string &context) {
    size_t brackets[256];
    new (brackets + '(') size_t{0};
    new (brackets + '[') size_t{0};
    new (brackets + '{') size_t{0};

    for (size_t index = 1; index < context.size(); ++index) {
      switch (context[index]) {
        case ',':
          if (brackets['('] || brackets['['] || brackets['{']) {
            continue;
          }

          return index;
        case '(':
          ++brackets['('];
          break;
        case ')':
          --brackets['('];
          break;
        case '[':
          ++brackets['['];
          break;
        case ']':
          --brackets[']'];
          break;
        case '{':
          ++brackets['{'];
          break;
        case '}':
          --brackets['{'];
          break;
        default: {
        }
      }
    }

    return -1;
  }

  inline std::string nextContext(const std::string &context) {
    size_t current = contextEnd(context) + 1;

    if (current == 0) {
      return "";
    }

    while (context[current] == ' ') {
      ++current;
    }

    return context.substr(current);
  }

  template<typename... Args>
  void variadicAssert(int, const std::string &, Args...) {}

  template<typename First, typename... Args>
  void variadicAssert(int line, const std::string &context, First first, Args... args) {
    if (!first) {
      throw TestError{concatenate("Assertion ", context.substr(0, contextEnd(context)), " on line ", line, " failed!")};
    }

    variadicAssert(line, nextContext(context), args...);
  }

  template<typename A, typename B>
  bool equalContainersIdx(const A &a, const B &b) {
    for (size_t i = 0; i < a.size(); ++i) {
      if (const_cast<A &>(a)[i] != const_cast<B &>(b)[i]) {
        return false;
      }

      if (const_cast<A &>(a).at(i) != const_cast<B &>(b).at(i)) {
        return false;
      }

      if (a[i] != b[i]) {
        return false;
      }

      if (a.at(i) != b.at(i)) {
        return false;
      }
    }

    return true;
  }

  template<template<size_t> typename SubTest, size_t CurrentChunkSize, size_t MaxChunkSize>
  void chainCall() {
    if constexpr (CurrentChunkSize <= MaxChunkSize) {
      SubTest<CurrentChunkSize>{}();
      chainCall<SubTest, CurrentChunkSize * 2, MaxChunkSize>();
    }
  }

  template<typename F, typename... Args>
  void timeFunction(F f, Args &&...message) {
    namespace chrono = std::chrono;
    const auto start = chrono::high_resolution_clock::now();
    f();
    const auto end = chrono::high_resolution_clock::now();
    const auto duration = end - start;

    print(std::forward<Args>(message)...);
    outputDuration(duration);
  }

  class Foo {
      static constexpr const char *s = "hi";
  };

  template<template<size_t> typename SubTest>
  void callSubTest() {
    ++testNumber;
    const char *name = SubTest<1>::name;
    std::cout << " (Test " << testNumber << ")\t" << name << '\t' << std::flush;
    try {
      timeFunction(chainCall<SubTest>, "\r (Test ", testNumber, ") ", name, ": ");
    } catch (...) {
      std::cout << std::endl;
      throw;
    }

    std::cout << '\n' << std::endl;
    assertLeakage();
  }

  template<typename F>
  void callPerformanceTest(F function, const char *name) {
    ++testNumber;
    std::cout << " (Test " << testNumber << ")\t" << name;
    std::cout.flush();
    timeFunction(function, ": ");
    std::cout << '\n' << std::endl;
  }

  template<std::integral Number>
  std::string ordinalize(Number n) {
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
} // namespace test_utility
