#include "../../include/ChunkedListDef.hpp"
#include "../../include/Tests.hpp"

#undef TEST_DEFS
#define TEST_DEFS CHUNKED_LIST_DEFS CHUNKED_LIST_ITERATOR_DEFS CHUNKED_LIST_SLICE_DEFS

SUBTEST(Initialization) {
  List{};
  List{BASIC_INITIALIZER_LIST};
}

SUBTEST(Pushing) {
  List list;
  Integral value{5};

  // const Integral &valueRef = value;

  for (size_t i = 0; i < static_cast<size_t>(value); ++i) {
    list.push_back(value);
  }
}

SUBTEST(Static_Members) {
  ASSERT(static_cast<bool>(std::is_same_v<typename List::value_type, Integral>))
  ASSERT(List::chunk_size == ChunkSize);

  struct ArbitraryType {};

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

SUBTEST(Member_Iteration_Methods) {
  List list{BASIC_INITIALIZER_LIST};
  const List &constRef = list;

  size_t counter = 0;

  // List::{begin, end, rbegin, rend}

  ASSERT_INCREMENT(list.begin(), list.end());

  DECREMENT_ASSERT(list.rbegin(), list.rend());

  // const List::{begin, end, rbegin, rend}

  ASSERT_INCREMENT(constRef.begin(), constRef.end(), const)

  DECREMENT_ASSERT(constRef.rbegin(), constRef.rend(), const)

  // List::{cbegin, cend, crbegin, crend}

  ASSERT_INCREMENT(list.cbegin(), list.cend(), const)

  DECREMENT_ASSERT(list.crbegin(), list.crend(), const)

  // const List::{cbegin, cend, crbegin, crend}

  ASSERT_INCREMENT(list.cbegin(), list.cend(), const)

  DECREMENT_ASSERT(constRef.crbegin(), constRef.crend(), const)
}

SUBTEST(Global_Iteration_Methods) {
  List list{BASIC_INITIALIZER_LIST};
  // const List &constRef = list;

  size_t counter = 0;

  ASSERT_INCREMENT(begin(list), end(list))

  DECREMENT_ASSERT(rbegin(list), rend(list))

  for (Integral i{0}; static_cast<size_t>(i) < list.size(); ++i) {
    ASSERT(list.at(i) == i)
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
