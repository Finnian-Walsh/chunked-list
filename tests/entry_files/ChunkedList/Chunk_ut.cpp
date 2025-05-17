
#include "../../include/ChunkedListDef.hpp"
#include "../../include/Tests.hpp"

#undef TEST_DEFS
#define TEST_DEFS ALIGNED_ARRAY_DEF CHUNK_DEFS

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

SUBTEST(Static_Members) {
  ASSERT(static_cast<bool>(std::is_same_v<Integral, typename Chunk::value_type>))
  ASSERT(ChunkSize == Chunk::chunk_size)

  struct ArbitraryType {};

  ASSERT(
    static_cast<bool>(std::is_same_v<typename Chunk::template allocator_type<ArbitraryType>, Allocator<ArbitraryType>>))
}

SUBTEST(Data_Accessing) {
  Vector vec(ChunkSize);
  std::iota(vec.begin(), vec.end(), 0);

  Chunk chunk{vec.data(), vec.size()};
  const Integral *data = chunk.data();

  for (size_t i = 0; i < ChunkSize; ++i) {
    ASSERT(data[i] == i)
  }
}

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

SUBTEST(Popping) {
  Chunk chunk;

  for (size_t i = 0; i < ChunkSize; ++i) {
    chunk.push_back(Integral{i});
  }

  for (size_t i = 0; i < ChunkSize; ++i) {
    chunk.pop_back();
  }
}

SUBTEST(Clearing) {
  Vector vec(ChunkSize);
  std::iota(vec.begin(), vec.end(), 0);

  AlignedArray<Chunk> chunkContainer;

  for (size_t i = 1; i <= ChunkSize; ++i) {
    chunkContainer.construct(0, vec.data(), ChunkSize / i);
    chunkContainer.load()->clear();
    chunkContainer.destroy(0);
  }
}

SUBTEST(Arithmetic) {
  constexpr size_t CHUNK_COUNT = 3;
  constexpr size_t ARRAY_SIZE = CHUNK_COUNT + 1;

  AlignedArray<Chunk, ARRAY_SIZE> array;

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

SUBTEST(Indexing) {
  Vector vec(ChunkSize);
  std::iota(vec.begin(), vec.end(), 0);

  Chunk chunk{vec.data(), ChunkSize};

  ASSERT_OBJ(&chunk, obj->size() == vec.size(), obj->prevChunk == nullptr, obj->nextChunk == nullptr,
             test_utility::equalContainersIdx(*obj, vec));
}

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

SUBTEST(Iteration) {
  AlignedArray<Chunk> chunk;
  Chunk dummy{chunk + 0, chunk + 0};

  Chunk &ref = *chunk;

  {
    Vector vec(ChunkSize);
    std::iota(vec.begin(), vec.end(), 0);
    chunk.construct(0, vec.data(), ChunkSize, &dummy, &dummy);
  }

  ASSERT_OBJ(chunk.load(), obj->begin() + ChunkSize == obj->end(), obj->end() - ChunkSize == obj->begin())

  size_t counter = ChunkSize;

  DECREMENT_ASSERT(ref.rbegin(), ref.rend());

  ASSERT_INCREMENT(ref.begin(), ref.end());

  const Chunk &cref = *chunk;

  DECREMENT_ASSERT(cref.rbegin(), cref.rend(), const);

  ASSERT_INCREMENT(cref.begin(), cref.end(), const);

  DECREMENT_ASSERT(cref.crbegin(), cref.crend(), const);

  ASSERT_INCREMENT(cref.cbegin(), cref.cend(), const);

  chunk.destroy(0);
}

UNIT_TEST(Chunk)
