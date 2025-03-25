#include "TestUtility.hpp"

#include <chrono>
#include <list>
#include <numeric>
#include <ranges>
#include <vector>

using namespace TestUtility;

int main() {
  constexpr size_t pushes = 1'000'000;
  {
    std::cout << "ChunkedList<size_t> push time test:" << std::endl;
    ChunkedList<size_t, pushes> testList;

    std::cout << "starting..." << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < pushes; i++) {
      testList.push(i);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration_s = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    const auto duration_ns = std::chrono::duration_cast<std::chrono::duration<double, std::nano>>(end - start);

    std::cout << "Total time taken to push " << pushes << " integers: " << duration_s.count() << " seconds\n";
    std::cout << "Average time to push 1 integer: " << duration_ns.count() / pushes << " nanoseconds\n" << std::endl;
  }
  {
    std::cout << "std::list<size_t> push time test" << std::endl;
    std::list<size_t> testList;

    std::cout << "starting..." << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < pushes; i++) {
      testList.push_back(i);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration_s = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    const auto duration_ns = std::chrono::duration_cast<std::chrono::duration<double, std::nano>>(end - start);

    std::cout << "Total time taken to push " << pushes << " integers: " << duration_s.count() << " seconds\n";
    std::cout << "Average time to push 1 integer: " << duration_ns.count() / pushes << " nanoseconds\n" << std::endl;
  }
  {
    std::cout << "std::vector<size_t> push time test" << std::endl;
    std::vector<size_t> testList;

    std::cout << "starting..." << std::endl;

    testList.reserve(pushes);

    const auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < pushes; i++) {
      testList.push_back(i);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration_s = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    const auto duration_ns = std::chrono::duration_cast<std::chrono::duration<double, std::nano>>(end - start);

    std::cout << "Total time taken to push " << pushes << " integers: " << duration_s.count() << " seconds\n";
    std::cout << "Average time to push 1 integer: " << duration_ns.count() / pushes << " nanoseconds\n" << std::endl;
  }

  return 0;
}
