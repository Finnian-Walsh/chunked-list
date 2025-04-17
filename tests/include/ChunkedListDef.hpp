#pragma once

#include "chunked_list/ChunkedList.hpp"
#include "chunked_list/ChunkedListAccessor.hpp"

using chunked_list::ChunkedList;

template<typename T, size_t N, template<typename> typename A>
using CurrentList = ChunkedList<T, N, A>;