
#include "../../include/ChunkedListDef.hpp"
#include "../../include/Tests.hpp"

#undef TEST_DEFS
#define TEST_DEFS CHUNK_DEFS

/*
 * Chunk(Chunk *prevChunk, Chunk *nextChunk = nullptr);
 * Chunk(const T *pointer, size_t size, Chunk *prevChunk = nullptr, Chunk *nextChunk = nullptr);
 * Chunk(T value, Chunk *prevChunk = nullptr, Chunk *nextChunk = nullptr);
 * Chunk();
 */
SUBTEST(Initialization) {
  Vector vec;

  for (size_t index = 0; index < Chunk::chunk_size; ++index) {
    vec.emplace_back(index);
  }

  auto prev = reinterpret_cast<Chunk *>(static_cast<uintptr_t>(1));
  auto next = reinterpret_cast<Chunk *>(static_cast<uintptr_t>(2));

  ASSERT_OBJ(Chunk{}, obj.prevChunk == nullptr, obj.nextChunk == nullptr)
  ASSERT_OBJ(Chunk{prev}, obj.prevChunk == prev, obj.nextChunk == nullptr);
  ASSERT_OBJ((Chunk{prev, next}), obj.prevChunk == prev, obj.nextChunk == next)

  ASSERT_OBJ((Chunk{vec.data(), vec.size(), prev, next}), obj.prevChunk == prev, obj.nextChunk == next)
  ASSERT_OBJ((Chunk{vec.data(), vec.size(), prev}), obj.prevChunk == prev, obj.nextChunk == nullptr)
  ASSERT_OBJ((Chunk{vec.data(), vec.size()}), obj.prevChunk == nullptr, obj.nextChunk == nullptr)

  ASSERT_OBJ(Chunk{Integral{}}, obj.prevChunk == nullptr, obj.nextChunk == nullptr)
  ASSERT_OBJ((Chunk{Integral{}, prev}), obj.prevChunk == prev, obj.nextChunk == nullptr)
  ASSERT_OBJ((Chunk{Integral{}, prev, next}), obj.prevChunk == prev, obj.nextChunk == next)
}

/*
 * value_type
 * chunk_size
 */
SUBTEST(StaticMembers){ASSERT(static_cast<bool>(std::is_same_v<Integral, typename Chunk::value_type>))
                         ASSERT(ChunkSize == Chunk::chunk_size)}

/*
 * T *data();
 * const T *data() const;
 */
SUBTEST(DataAccessing) {
  Vector vec(ChunkSize);
  std::iota(vec.begin(), vec.end(), 0);

  Chunk chunk{vec.data(), vec.size()};
  const Integral *data = chunk.data();

  for (size_t i = 0; i < ChunkSize; ++i) {
    ASSERT(data[i] == i)
  }
}

// void push_back(T value);
SUBTEST(Pushing) {
  Chunk chunk1;

  for (size_t i = 0; i < ChunkSize; ++i) {
    chunk1.push_back(Integral{i});
  }

  Chunk chunk2;

  for (size_t i = 0; i < ChunkSize / 2; ++i) {
    chunk2.push_back(Integral{i});
  }
}

/*
 * template<typename... Args>
 *  requires utility::can_construct<T, Args...>
 * void emplace_back(Args &&...args);
 */
SUBTEST(Emplacing) {
  Chunk chunk1;

  for (size_t i = 0; i < ChunkSize; ++i) {
    chunk1.emplace_back(i);
  }

  Chunk chunk2;

  for (size_t i = 0; i < ChunkSize / 2; ++i) {
    chunk2.emplace_back(i);
  }
}

// void pop_back();
SUBTEST(Popping) {
  Chunk chunk;

  for (size_t i = 0; i < ChunkSize; ++i) {
    chunk.push_back(Integral{i});
  }

  for (size_t i = 0; i < ChunkSize; ++i) {
    chunk.pop_back();
  }
}

// void clear();
SUBTEST(Clearing) {
  Vector vec(ChunkSize);
  std::iota(vec.begin(), vec.end(), 0);

  Chunk chunk{vec.data(), ChunkSize};

  for (size_t i = 1; i <= ChunkSize; ++i) {
    chunk.clear();
    ChunkAllocatorTraits::destroy(chunk_allocator, &chunk);
    new (&chunk) Chunk{vec.data(), ChunkSize / i};
  }
}

/*
 * Chunk &operator+(size_t n);
 * Chunk &operator-(size_t n);
 */
SUBTEST(Arithmetic) {
  constexpr size_t CHUNK_COUNT = 3;
  constexpr size_t ARRAY_SIZE = CHUNK_COUNT + 1;

  test_utility::AlignedArray<Chunk, ARRAY_SIZE> array;

  Chunk *sentinel = array + CHUNK_COUNT;

  // Construction

  array.construct(CHUNK_COUNT, sentinel - 1, array.load());

  array.construct(0, sentinel, array + 1);

  for (size_t i = 1; i < CHUNK_COUNT; ++i) {
    array.construct(i, array + (i - 1), array + (i + 1));
  }

  ASSERT_OBJ(sentinel, &obj->operator+(ARRAY_SIZE) == obj, &obj->operator-(ARRAY_SIZE) == obj)

  // Destruction

  for (size_t i = 0; i < ARRAY_SIZE; ++i) {
    array[i].~Chunk();
  }
}

/*
 * size_t size() const;
 * bool empty() const;
 */
SUBTEST(Size) {
  Chunk chunk;

  ASSERT(chunk.size() == 0)
  ASSERT(chunk.empty())

  for (size_t i = 0; i < ChunkSize; ++i) {
    ASSERT(chunk.size() == i)
    chunk.push_back(Integral{i});
  }

  for (size_t i = ChunkSize; i > 0; --i) {
    ASSERT(chunk.size() == i);
    chunk.pop_back();
  }

  ASSERT(chunk.empty())
}

/*
 *T &operator[](size_t index);
 * const T &operator[](size_t index) const;
 * T &at(size_t index);
 * const T &at(size_t index) const;
 */
SUBTEST(Indexing) {
  Vector vec(ChunkSize);
  std::iota(vec.begin(), vec.end(), 0);

  Chunk chunk{vec.data(), ChunkSize};

  ASSERT_OBJ(&chunk, obj->size() == vec.size(), obj->prevChunk == nullptr, obj->nextChunk == nullptr,
             test_utility::equalContainersIdx(*obj, vec));
}

/*
 * bool operator==(const Chunk &other) const;
 * bool operator!=(const Chunk &other) const;
 */
SUBTEST(Comparison) {
  Chunk chunk1, chunk2;

  ASSERT(chunk1 == chunk2)

  for (size_t i = 0; i < ChunkSize; ++i) {
    chunk1.emplace_back(i);
    ASSERT(chunk1 != chunk2)
    chunk2.emplace_back(i);
    ASSERT(chunk1 == chunk2)
  }

  chunk1.prevChunk = &chunk2;
  chunk1.nextChunk = &chunk2;

  chunk2.prevChunk = &chunk1;
  chunk2.nextChunk = &chunk1;

  ASSERT(chunk1 == chunk2)

  for (size_t i = 0; i < ChunkSize; ++i) {
    chunk1.pop_back();
    ASSERT(chunk1 != chunk2)
    chunk2.pop_back();
    ASSERT(chunk1 == chunk2)
  }
}

/*
 * iterator begin();
 * const_iterator cbegin() const;
 * reverse_iterator rbegin();
 * const_reverse_iterator crbegin() const;
 * iterator end();
 * const_iterator cend() const;
 * reverse_iterator rend();
 * const_reverse_iterator crend() const;
 */
SUBTEST(Iteration) {
  test_utility::AlignedArray<Chunk> chunk;
  Chunk dummy{chunk + 0, chunk + 0};

  Chunk &ref = *chunk;

  {
    Vector vec(ChunkSize);
    std::iota(vec.begin(), vec.end(), 0);
    chunk.construct(0, vec.data(), ChunkSize, &dummy, &dummy);
  }

  ASSERT_OBJ(chunk.load(), obj->begin() + ChunkSize == obj->end(), obj->end() - ChunkSize == obj->begin())

  size_t count = ChunkSize;

  std::for_each(ref.rbegin(), ref.rend(), [&count](Integral &n) {
    --count;
    ASSERT(n == count);
    ++n;
  });

  std::for_each(ref.begin(), ref.end(), [&count](Integral &n) {
    ++count;
    ASSERT(n == count)
    --n;
  });

  const Chunk &cref = chunk.load();

  std::for_each(cref.crbegin(), cref.crend(), [&count](const Integral &n) {
    --count;
    ASSERT(n == count);
  });

  std::for_each(cref.cbegin(), cref.cend(), [&count](const Integral &n) {
    ASSERT(n == count)
    ++count;
  });

  chunk.destroy(0);
}

UNIT_TEST(Chunk)
