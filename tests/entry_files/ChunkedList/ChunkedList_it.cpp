#include "../../include/ChunkedListDef.hpp"
#include "../../include/Tests.hpp"

#undef TEST_DEFS
#define TEST_DEFS CHUNKED_LIST_DEFS CHUNKED_LIST_SLICE_DEFS

#define BASIC_INITIALIZER_LIST IntegralInitializerList{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}

SUBTEST(Initialization) {
  List{};
  List{BASIC_INITIALIZER_LIST};
}

SUBTEST(Pushing) {
  List list;
  Integral value{5};

  // const Integral &valueRef = value;

  for (size_t i = 0; i < static_cast<size_t>(value); ++i) {
    list.push_back(std::forward<Integral>(value));
  }
}

SUBTEST(StaticMembers) {
  ASSERT(static_cast<bool>(std::is_same_v<typename List::value_type, Integral>))
  ASSERT(List::chunk_size == ChunkSize);

  struct ArbitraryType {};

  ASSERT(
    static_cast<bool>(std::is_same_v<typename List::template allocator_type<ArbitraryType>, Allocator<ArbitraryType>>))
  ASSERT(
    static_cast<bool>(std::is_same_v<typename List::template allocator_type<ArbitraryType>, Allocator<ArbitraryType>>))
}

SUBTEST(Indexing) {
  List list{BASIC_INITIALIZER_LIST};
  const List &constRef = list;

  for (size_t i = 0; i < list.size(); ++i) {
    ASSERT(list[i] == i)
    ASSERT(list.at(i) == i)
    ASSERT(constRef[i] == i)
    ASSERT(constRef.at(i) == i)
  }
}

SUBTEST(Iteration) {
  List list{BASIC_INITIALIZER_LIST};
  const List &constRef = list;

  size_t counter = 0;

  // List::{begin, end, rbegin, rend}

  for (auto it = list.begin(); it != list.end(); ++it) {
    ASSERT(*it == counter)
    ++counter;
  }

  for (auto rit = list.rbegin(); rit != list.rend(); ++rit) {
    --counter;
    ASSERT(*rit == counter)
  }

  // const List::{begin, end, rbegin, rend}

  for (auto it = constRef.begin(); it != constRef.end(); ++it) {
    ASSERT(*it == counter)
    ++counter;
  }

  for (auto it = constRef.rbegin(); it != constRef.rend(); ++it) {
    --counter;
    ASSERT(*it == counter)
  }

  // List::{cbegin, cend, crbegin, crend}

  for (auto cit = list.cbegin(); cit != list.cend(); ++cit) {
    ASSERT(*cit == counter)
    ++counter;
  }

  for (auto crit = list.crbegin(); crit != list.crend(); ++crit) {
    --counter;
    ASSERT(*crit == counter)
  }

  // const List::{cbegin, cend, crbegin, crend}

  for (auto cit = constRef.cbegin(); cit != constRef.cend(); ++cit) {
    ASSERT(*cit == counter)
    ++counter;
  }

  for (auto crit = constRef.crbegin(); crit != constRef.crend(); ++crit) {
    --counter;
    ASSERT(*crit == counter)
  }
}

SUBTEST(Slicing) {
  List list{BASIC_INITIALIZER_LIST};
  // const List &constRef = list;

  MutableSlice slice = list.slice(list.begin(), list.end());
  ASSERT(slice.begin() + list.size() == slice.end())
  ASSERT(slice.end() - list.size() == slice.begin())
}

INTEGRATION_TEST(ChunkedList)
