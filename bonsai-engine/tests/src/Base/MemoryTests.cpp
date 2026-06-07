#include <gtest/gtest.h>
#include <BonsaiBase.hpp>

static constexpr usize DEFAULT_TEST_ALIGNMENT = 128;

TEST(MemoryTests, TestSimpleAllocate)
{
    void* block = Memory::Malloc(1024, DEFAULT_TEST_ALIGNMENT);
    ASSERT_NE(block, nullptr);

    uintptr blockAddr = reinterpret_cast<uintptr>(block);
    ASSERT_EQ(blockAddr, blockAddr & ~(DEFAULT_TEST_ALIGNMENT - 1));

    Memory::Free(block);
}

TEST(MemoryTests, TestAllocateNotMultipleOfAlignment)
{
    void* block = Memory::Malloc(1234, 32);
    ASSERT_NE(block, nullptr);

    uintptr blockAddr = reinterpret_cast<uintptr>(block);
    ASSERT_EQ(blockAddr, blockAddr & ~(32 - 1));

    Memory::Free(block);
}

TEST(MemoryTests, TestSimpleArrayAllocate)
{
    void* block = Memory::Calloc(5, 1024, DEFAULT_TEST_ALIGNMENT);
    ASSERT_NE(block, nullptr);

    uintptr blockAddr = reinterpret_cast<uintptr>(block);
    ASSERT_EQ(blockAddr, blockAddr & ~(DEFAULT_TEST_ALIGNMENT - 1));

    Memory::Free(block);
}

TEST(MemoryTests, TestSimpleReallocate)
{
    void* block = Memory::Malloc(1024, DEFAULT_TEST_ALIGNMENT);
    ASSERT_NE(block, nullptr);

    void* newBlock = Memory::Realloc(block, 2048, DEFAULT_TEST_ALIGNMENT);
    ASSERT_NE(newBlock, nullptr);

    uintptr blockAddr = reinterpret_cast<uintptr>(newBlock);
    ASSERT_EQ(blockAddr, blockAddr & ~(DEFAULT_TEST_ALIGNMENT - 1));

    Memory::Free(newBlock);
}

TEST(MemoryTests, TestReallocateDifferentAlignment)
{
    void* block = Memory::Malloc(1024, 64);
    ASSERT_NE(block, nullptr);

    void* newBlock = Memory::Realloc(block, 1024, 16);
    ASSERT_NE(newBlock, nullptr);

    uintptr blockAddr = reinterpret_cast<uintptr>(newBlock);
    ASSERT_EQ(blockAddr, blockAddr & ~(16 - 1));

    Memory::Free(newBlock);
}
