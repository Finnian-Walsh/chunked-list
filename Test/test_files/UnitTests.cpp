#include "../core/TestUtility.hpp"

#include <chrono>
#include <ranges>
#include <numeric>
#include <vector>
#include <list>

using namespace TestUtility;

int main() {
  constexpr size_t pushes = 1'000'000; {
    std::cout << "ChunkedList<int> push time test:" << std::endl;
    ChunkedList<int, pushes> testList;

    std::cout << "starting..." << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < pushes; i++) {
      testList.push(i);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration_s = std::chrono::duration_cast<std::chrono::duration<double> >(end - start);
    const auto duration_ns = std::chrono::duration_cast<std::chrono::duration<double, std::nano> >(end - start);

    std::cout << "Total time taken to push " << pushes << " integers: " << duration_s.count() << " seconds\n";
    std::cout << "Average time to push 1 integer: " << duration_ns.count() / pushes << " nanoseconds\n" << std::endl;
  } {
    std::cout << "std::list<int> push time test" << std::endl;
    std::list<int> testList;

    std::cout << "starting..." << std::endl;

    const auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < pushes; i++) {
      testList.push_back(i);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration_s = std::chrono::duration_cast<std::chrono::duration<double> >(end - start);
    const auto duration_ns = std::chrono::duration_cast<std::chrono::duration<double, std::nano> >(end - start);

    std::cout << "Total time taken to push " << pushes << " integers: " << duration_s.count() << " seconds\n";
    std::cout << "Average time to push 1 integer: " << duration_ns.count() / pushes << " nanoseconds\n" << std::endl;
  } {
    std::cout << "std::vector<int> push time test" << std::endl;
    std::vector<int> testList;

    std::cout << "starting..." << std::endl;

    testList.reserve(pushes);

    const auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < pushes; i++) {
      testList.push_back(i);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration_s = std::chrono::duration_cast<std::chrono::duration<double> >(end - start);
    const auto duration_ns = std::chrono::duration_cast<std::chrono::duration<double, std::nano> >(end - start);

    std::cout << "Total time taken to push " << pushes << " integers: " << duration_s.count() << " seconds\n";
    std::cout << "Average time to push 1 integer: " << duration_ns.count() / pushes << " nanoseconds\n" << std::endl;
  }

  return 0;
}
