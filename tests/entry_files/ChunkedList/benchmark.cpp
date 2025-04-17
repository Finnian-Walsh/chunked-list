#include "ChunkedListDef.hpp"
#include "Tests.hpp"

#include <chrono>
#include <list>
#include <numeric>
#include <ranges>

PERFORMANCE_BENCHMARK("ChunkedList") {
  using namespace test_utility;
  using chunked_list::ChunkedList;

  using List = ChunkedList<size_t, 2, CustomAllocator>;

  AlignedArray<List> list;

  callPerformanceTest([&list]() mutable -> void { list.construct(0); }, "Constructing a list");

  constexpr int PUSHES = 4;

  callPerformanceTest(
    [&list]{
      for (int i = 0; i < PUSHES; ++i) {
        list->push_back(i);
      }
    },
    concatenate("Pushing ", PUSHES, " ints").c_str());

  callPerformanceTest(
    [&list] {
      for (int i = 0; i < PUSHES; ++i) {
        list->pop_back();
      }
    },
    concatenate("Popping ", PUSHES, " ints").c_str());

  callPerformanceTest([&list] { list.destroy(0); }, "Destroying");
}
