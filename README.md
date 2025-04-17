## Table of Contents

1. [Concept](#concept)
    - [Template parameters](#template-parameters)
    - [Deallocation](#deallocation)
2. [Chunks](#chunks)
3. [Methods](#methods)
    - [Iteration](#iteration)
    - [Sorting](#sorting)
    - [Private Member Accessing](#private-member-accessing)
4. [Installation](#installation)
5. [Examples](#examples)
    - [Basic usage](#basic-usage)
    - [String Concatenation](#string-concatenation)

# Chunked List

## Concept

The **ChunkedList** is a simple and efficient C++ list data structure, which uses **Chunks** to store data.

```cpp
template<typename T, size_t ChunkSize = 32, template<typename> typename Allocator = std::allocator>
class ChunkedList;
```

This data structure is essentially a linked list, where each node is a **Chunk**.

### Template parameters

- `T` the type of data which will be stored in the **ChunkedList**
- `ChunkSize` the size of each **Chunk**
- `Allocator` the template allocator class used for the allocation of chunks.

### Deallocation

When a **ChunkedList** instance is deallocated, every **Chunk** gets deallocated in reverse order of allocation.

## Chunks

Each **Chunk** contains a fixed array, of size ChunkSize. However, **Chunks** are abstracted away and from the user's
view.

Furthermore, each **Chunk** keeps track of its next index, incrementing and decrementing it by 1 each push and pop,
respectively.
Therefore, calling the pop method on a **ChunkedList** doesn't deallocate anything unless the next index of the back
**Chunk** is equal to 0, causing the entire **Chunk** to be deallocated. Using popped values which haven't been
deallocated is recommended against.

## Methods

### Iteration

To iterate over a **ChunkedList**, **Iterators** should be used - range-based for-loops are supported. Index based
for-loops should not be used, since each subscript operation performs a O(n) search, iterating through each **Chunk**
until the correct value is found.

```cpp
for (T value : chunkedList) {
  ...
}
```

### Sorting

The **ChunkedList** data structure comes with a built-in sort function, allowing you to sort it with a specified compare
class and an algorithm of your choice with template parameters.

```cpp
template<typename Compare = std::less<T>, utility::SortType = QuickSort>
void sort();
```

By default, the sort function uses `std::less<T>` to compare types and `QuickSort` as the Sorting algorithm.

### Private member accessing

The **ChunkedListAccessor** template class provides access to the non-public members:

- chunk_count (number of chunks)
- ValueAllocator (the T specialisation of the Allocator template template type parameter)
- ValueAllocatorTraits (the std wrapper for the ValueAllocator class)
- ChunkAllocator (the Chunk specialisation of the Allocator template template type parameter)
- ChunkAllocatorTraits (the std wrapper for the ChunkAllocator class)
- chunk_allocator (the ChunkAllocator object used for the allocation and deallocation of Chunks)
- sentinel (chunk before the front and after the back)
- generic_chunk_iterator (a template class used for defining chunk_iterator and const_chunk_iterator)
- generic_iterator (a template class used for defining iterator and const_iterator)

```cpp
template<typename T, size_t ChunkSize, typename Allocator>
class ChunkedListAccessor final : ChunkedList<T, ChunkSize, Allocator>;
```

Usage:

```cpp
using namespace chunked_list;

auto &accessor = static_cast<ChunkedListAccessor<T, ChunkSize, Allocator>>(list);

auto front = accessor.getFront();
auto back = accessor.getBack();
size_t chunkCount = accessor.getChunkCount();
```

Furthermore, the **Accessor** type aliases the **ChunkedListAccessor** for any given **ChunkedList**

```cpp
template<typename ChunkedListType>
using Accessor = ChunkedListAccessor<typename ChunkedListType::value_type, ChunkedListType::chunk_size,
typename ChunkedListType::allocator_type>;
```

## Installation

Simply clone the repository or add it as a submodule to your project.

```bash
git submodule add https://github.com/Finnian-Walsh/ChunkedList.git <path>
```

```bash
git clone https://github.com/Finnian-Walsh/ChunkedList.git
```

Then, add the `include` directory to your include directories.

## Examples

### Basic usage

```cpp
#include <iostream>

#include "chunked_list/ChunkedList.hpp"

using chunked_list::ChunkedList;

int main() {
  ChunkedList<int> list{1, 2, 3, 4, 5};
  
  for (int num: list)
    std::cout << num << '\n';    
    
  return 0;
}
```

Output:

```
1
2
3
4
5
```

### String Concatenation

```cpp
#include <iostream>

#include "chunked_list/ChunkedList.hpp"

using chunked_list::ChunkedList;

int main() {
  ChunkedList<std::string> list{"Hello", "world!"};
  
  std::cout << list.concat(" ") << std::endl;
}
```

Output:

```
Hello world!
```
