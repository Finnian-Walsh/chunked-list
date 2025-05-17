#pragma once

#include "chunked_list/ChunkedList.hpp"
#include "chunked_list/ChunkedListAccessor.hpp"

using chunked_list::ChunkedList;

template<typename T, size_t N, template<typename> typename A>
using CurrentList = ChunkedList<T, N, A>;

#define BASIC_INITIALIZER_LIST IntegralInitializerList{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
