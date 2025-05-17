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

  AlignedArray<List> listContainer;

  callPerformanceTest([&listContainer]() mutable -> void { listContainer.construct(0); }, "Constructing a list");

  constexpr int PUSHES = 4;

  callPerformanceTest(
    [&listContainer]{
      for (int i = 0; i < PUSHES; ++i) {
        listContainer.load()->push_back(i);
      }
    },
    concatenate("Pushing ", PUSHES, " ints").c_str());

  callPerformanceTest(
    [&listContainer] {
      for (int i = 0; i < PUSHES; ++i) {
        listContainer.load()->pop_back();
      }
    },
    concatenate("Popping ", PUSHES, " ints").c_str());

  callPerformanceTest([&listContainer] { listContainer.destroy(0); }, "Destroying");
}
