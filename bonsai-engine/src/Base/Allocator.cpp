#include "Allocator.hpp"

#include "Memory.hpp"

void* DefaultAllocator::Allocate(usize size, usize alignment)
{
    return Memory::Malloc(size, alignment);
}

void DefaultAllocator::Deallocate(void* ptr)
{
    Memory::Free(ptr);
}
