#include <gtest/gtest.h>
#include <BonsaiBase.hpp>

struct FooStruct
{
    int value = 0x42;
};

TEST(AllocatorTests, TestConstructDestruct)
{
    DefaultAllocator allocator{};
    void* ptr = allocator.Allocate(sizeof(FooStruct), alignof(FooStruct));
    EXPECT_NE(ptr, nullptr);

    FooStruct* fooStruct = Construct<FooStruct>(ptr);
    EXPECT_EQ(fooStruct->value, 0x42);

    Destruct(fooStruct);
    allocator.Deallocate(fooStruct);
}

TEST(AllocatorTests, TestConstructDestructArgs)
{
    DefaultAllocator allocator{};
    void* ptr = allocator.Allocate(sizeof(FooStruct), alignof(FooStruct));
    EXPECT_NE(ptr, nullptr);

    FooStruct* fooStruct = Construct<FooStruct>(ptr,  FooStruct{ 0xFF });
    EXPECT_EQ(fooStruct->value, 0xFF);
    
    Destruct(fooStruct);
    allocator.Deallocate(fooStruct);
}

TEST(AllocatorTests, TestAllocateArray)
{
    DefaultAllocator allocator{};
    void* ptr = allocator.Allocate(sizeof(int) * 32, alignof(int));
    EXPECT_NE(ptr, nullptr);

    int* array = ConstructArray<int>(ptr, 32);
    for (int i = 0; i < 32; i++) {
        EXPECT_EQ(array[i], 0);
    }

    DestructArray(array, 32);
    allocator.Deallocate(array);
}
