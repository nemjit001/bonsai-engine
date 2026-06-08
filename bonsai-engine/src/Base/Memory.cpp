#include "Memory.hpp"

#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <new>
#include "Assert.hpp"
#include "DebugOutput.hpp"
#include "PlatformDefines.hpp"

/// @brief Enable this macro to print memory allocation traces. This WILL bloat logging output, so use sparingly!
#define BONSAI_TRACE_ALLOCATIONS        0

/// @brief Enable this macro to override new and delete operators
#define BONSAI_OVERRIDE_NEW_AND_DELETE  1

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

void* operator new[] (std::size_t size)
{
    void* ptr = Memory::Malloc(size);
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }

    return ptr;
}

void* operator new[] (std::size_t size, std::align_val_t alignment)
{
    void* ptr = Memory::Malloc(size, static_cast<usize>(alignment));
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }

    return ptr;
}

void operator delete (void* ptr) noexcept
{
    Memory::Free(ptr);
}

void operator delete (void* ptr, [[maybe_unused]] std::align_val_t alignment) noexcept
{
    Memory::Free(ptr);
}

void operator delete[] (void* ptr) noexcept
{
    Memory::Free(ptr);
}

void operator delete[] (void* ptr, [[maybe_unused]] std::align_val_t alignment) noexcept
{
    Memory::Free(ptr);
}

#endif  //BONSAI_OVERRIDE_NEW_AND_DELETE

// --- End memory operator overrides ---

namespace Memory
{
    /// @brief Header for memory allocations.
    struct AllocationHeader
    {
        usize alignmentOffset;
        usize usableSize;
    };

    /// @brief Align a size or memory address to a byte boundary.
    /// @param value Value to align.
    /// @param alignment Alignment, must be a multiple of 2 and non-zero.
    /// @return 
    constexpr usize AlignUp(usize value, usize alignment)
    {
        BONSAI_ASSERT(alignment > 0 && (alignment & (alignment - 1)) == 0 && "Alignment should be non-zero and a power of 2!");
        return (value + alignment - 1) & ~(alignment - 1);
    }

    void* Malloc(usize size, usize alignment)
    {
        if (size == 0) {
            return nullptr;
        }

        usize const totalSize = sizeof(AllocationHeader) + size + alignment - 1;
        void* ptr = std::malloc(totalSize);
        if (ptr == nullptr) {
            return nullptr;
        }

        uintptr const baseAddress = reinterpret_cast<uintptr>(ptr);
        uintptr const alignedAddress = AlignUp(baseAddress + sizeof(AllocationHeader), alignment);
        ptr = reinterpret_cast<void*>(alignedAddress);

        AllocationHeader* header = &static_cast<AllocationHeader*>(ptr)[-1];
        header->alignmentOffset = alignedAddress - baseAddress;
        header->usableSize = size;

    #if     BONSAI_TRACE_ALLOCATIONS
        PrintDebugString("Malloc: block %p of %zu bytes (alignment: %zu)", ptr, size, alignment);
    #endif  //BONSAI_TRACE_ALLOCATIONS
        return ptr;
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
        // XXX(nemjit001): This is a simple pessimistic implementation of an aligned realloc that works with the aligned malloc implementation.
        usize const oldSize = UsableSize(block);
        uintptr const address = reinterpret_cast<uintptr>(block);

        // Reallocate memory block if resizing to a larger size or alignment does not match
        void* newBlock = block;
        if (size > oldSize || AlignUp(address, alignment) != address)
        {
            newBlock = Malloc(size, alignment);
            if (newBlock == nullptr) {
                return nullptr;
            }

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

        AllocationHeader const* header = &static_cast<AllocationHeader*>(block)[-1];
        void* original = reinterpret_cast<void*>(reinterpret_cast<uintptr>(block) - header->alignmentOffset);
        std::free(original);
    }

    usize UsableSize(void* block)
    {
        if (block == nullptr) {
            return 0;
        }

        AllocationHeader const* header = &static_cast<AllocationHeader*>(block)[-1];
        return header->usableSize;
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

