#include "Memory.hpp"

#include <cstdlib>
#include <cstring>
#include <new>
#include "Assert.hpp"
#include "DebugOutput.hpp"
#include "PlatformDefines.hpp"

/// @brief Enable this macro to print memory allocation traces. This WILL bloat logging output, so use sparingly!
#define BONSAI_TRACE_ALLOCATIONS        0

/// @brief Enable this macro to override new and delete operators
#define BONSAI_OVERRIDE_NEW_AND_DELETE  0

// --- Memory operator overrides ---

#if     BONSAI_OVERRIDE_NEW_AND_DELETE //FIXME(nemjit001): For some reason gtest crashes when discovering tests on win32 with custom new/delete operators that use _aligned_malloc :(
void* operator new (std::size_t size)
{
    void* ptr = Memory::Malloc(size);
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }

    return ptr;
}

void* operator new (std::size_t size, std::align_val_t alignment)
{
    void* ptr = Memory::Malloc(size, static_cast<usize>(alignment));
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }

    return ptr;
}

void operator delete(void* ptr, [[maybe_unused]] std::align_val_t alignment) noexcept
{
    Memory::Free(ptr);
}
#endif  //BONSAI_OVERRIDE_NEW_AND_DELETE

// --- End memory operator overrides ---

namespace Memory
{
    struct Header
    {
        usize offset;   // Alignment offset from aligned pointer back to original pointer.
        usize size;     // Allocation size in bytes as specified by user. NOT the allocation resident size.
    };

    /// @brief Align a size or memory address to a byte boundary.
    /// @param value Value to align.
    /// @param alignment Alignment, must be a multiple of 2 and non-zero.
    /// @return 
    constexpr usize AlignUp(usize value, usize alignment)
    {
        BONSAI_ASSERT(alignment > 0 && alignment % 2 == 0 && "Alignment should be non-zero and a multiple of 2!");
        return (value + alignment - 1) & ~(alignment - 1);
    }

    void* Malloc(usize size, usize alignment)
    {
        if (size == 0 || alignment % 2 != 0) {
            return nullptr;
        }

        usize const alignedSize = AlignUp(sizeof(Header) + size, alignment);
        void* block = std::malloc(alignedSize);
        void* alignedBlock = reinterpret_cast<void*>(AlignUp(reinterpret_cast<usize>(block) + sizeof(Header), alignment));
        Header& header = static_cast<Header*>(alignedBlock)[-1];
        header.offset = reinterpret_cast<usize>(alignedBlock) - reinterpret_cast<usize>(block);
        header.size = size;

    #if     BONSAI_TRACE_ALLOCATIONS
        PrintDebugString("Malloc: block %p is %zu bytes (alignment: %zu)", alignedBlock, size, alignment);
    #endif  //BONSAI_TRACE_ALLOCATIONS
        return alignedBlock;
    }

    void* Malloc(usize size)
    {
        return Malloc(size, DEFAULT_NEW_ALIGNMENT);
    }

    void* Calloc(usize count, usize size, usize alignment)
    {
        usize const blockSize = count * size;
        void* block = Malloc(blockSize, alignment);
        if (block == nullptr) {
            return nullptr;
        }

        MemSet(block, 0, blockSize);
    #if     BONSAI_TRACE_ALLOCATIONS
        PrintDebugString("Calloc: block %p of %zu bytes (alignment: %zu)", block, blockSize, alignment);
    #endif  //BONSAI_TRACE_ALLOCATIONS
        return block;
    }

    void* Calloc(usize count, usize size)
    {
        return Calloc(count, size, DEFAULT_NEW_ALIGNMENT);
    }

    void* Realloc(void* block, usize size, usize alignment)
    {
        // XXX(nemjit001): This is a simple pessimistic implementation of an aligned realloc since Linux does not provide this function in its standard lib.
        usize const oldSize = MallocSize(block);
        uintptr const address = reinterpret_cast<uintptr>(block);

        // Reallocate memory block if resizing to a larger size or alignment does not match
        void* newBlock = block;
        if (size > oldSize || AlignUp(address, alignment) != address)
        {
            newBlock = Malloc(size, alignment);

            MemCopy(newBlock, block, oldSize);
            Free(block);
        }

    #if     BONSAI_TRACE_ALLOCATIONS
        PrintDebugString("Realloc: block %p was %p and is %zu bytes (alignment: %zu)", newBlock, block, size, alignment);
    #endif  //BONSAI_TRACE_ALLOCATIONS
        return newBlock;
    }

    void* Realloc(void* block, usize size)
    {
        return Realloc(block, size, DEFAULT_NEW_ALIGNMENT);
    }

    void Free(void* block)
    {
    #if     BONSAI_TRACE_ALLOCATIONS
        PrintDebugString("Free: block %p", block);
    #endif  //BONSAI_TRACE_ALLOCATIONS

        if (block == nullptr) {
            return;
        }

        Header const& header = static_cast<Header*>(block)[-1];
        void* original = reinterpret_cast<void*>(reinterpret_cast<uintptr>(block) - header.offset);
        std::free(original);
    }

    usize MallocSize(void* block)
    {
        if (block == nullptr) {
            return 0;
        }

        Header const& header = static_cast<Header*>(block)[-1];
        return header.size;
    }

    void MemSet(void* block, int value, usize size)
    {
        std::memset(block, value, size);
    }

    void MemCopy(void* dest, void* src, usize size)
    {
        std::memcpy(dest, src, size);
    }

    void MemMove(void* dest, void* src, usize size)
    {
        std::memmove(dest, src, size);
    }
} // namespace Memory

