#include "Memory.hpp"

#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <new>
#include "Assert.hpp"
#include "DebugOutput.hpp"
#include "PlatformDefines.hpp"

/// @brief Enable this macro to print memory allocation traces. This WILL bloat logging output, so use sparingly!
#define BONSAI_TRACE_ALLOCATIONS    (0 && !BONSAI_BUILD_RELEASE)

// --- Memory operator overrides ---

void* operator new (std::size_t size)
{
    void* block = Memory::Malloc(size);
    if (block == nullptr) {
        throw std::bad_alloc();
    }

    return block;
}

void* operator new (std::size_t size, std::align_val_t alignment)
{
    void* block = Memory::Malloc(size, static_cast<usize>(alignment));
    if (block == nullptr) {
        throw std::bad_alloc();
    }

    return block;
}

void operator delete(void* ptr, [[maybe_unused]] std::align_val_t alignment) noexcept
{
    Memory::Free(ptr);
}

// --- End memory operator overrides ---

namespace Memory
{
    /// @brief Get the minimum alignment for a given size in bytes.
    /// @param size Size in bytes for query.
    /// @return The minimum alignment for the size.
    constexpr usize GetAlignment(usize size)
    {
        if (size <= sizeof(uint8)) {
            return alignof(uint8);
        }
        else if (size <= sizeof(uint16)) {
            return alignof(uint16);
        }
        else if (size <= sizeof(uint32)) {
            return alignof(uint32);
        }
        else if (size <= sizeof(uint64)) {
            return alignof(uint64);
        }

        return alignof(std::max_align_t);
    }

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
    #if     BONSAI_PLATFORM_WINDOWS
        void* block = _aligned_malloc(size, alignment);
    #elif BONSAI_PLATFORM_LINUX
        size = AlignUp(size, alignment);
        void* block = std::aligned_alloc(alignment, size);
    #else
        void* block = nullptr;
    #endif

#if     BONSAI_TRACE_ALLOCATIONS
        PrintDebugString("Malloc: block %p is %zu bytes (alignment: %zu)", block, size, alignment);
#endif  //BONSAI_TRACE_ALLOCATIONS
        return block;
    }

    void* Malloc(usize size)
    {
        return Malloc(size, GetAlignment(size));
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
        PrintDebugString("Calloc: block %p of %zu bytes with size %zu and count %zu (alignment: %zu)", block, blockSize, size, count, alignment);
#endif  //BONSAI_TRACE_ALLOCATIONS
        return block;
    }

    void* Calloc(usize count, usize size)
    {
        return Calloc(count, size, GetAlignment(count * size));
    }

    void* Realloc(void* block, usize size, usize alignment)
    {
    #if     BONSAI_PLATFORM_WINDOWS
        void* newBlock = _aligned_realloc(block, size, alignment);
    #elif   BONSAI_PLATFORM_LINUX
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
    #else
        void* newBlock = nullptr;
    #endif

#if     BONSAI_TRACE_ALLOCATIONS
        PrintDebugString("Realloc: block %p was %p and is %zu bytes (alignment: %zu)", newBlock, block, size, alignment);
#endif  //BONSAI_TRACE_ALLOCATIONS
        return newBlock;
    }

    void* Realloc(void* block, usize size)
    {
        return Realloc(block, size, GetAlignment(size));
    }

    void Free(void* block)
    {
#if     BONSAI_TRACE_ALLOCATIONS
        PrintDebugString("Free: block %p", block);
#endif  //BONSAI_TRACE_ALLOCATIONS

    #if     BONSAI_PLATFORM_WINDOWS
        _aligned_free(block);
    #elif   BONSAI_PLATFORM_LINUX
        std::free(block);
    #endif
    }

    usize MallocSize(void* block)
    {
    #if     BONSAI_PLATFORM_WINDOWS
        return _msize(block);
    #elif   BONSAI_PLATFORM_LINUX
        return malloc_usable_size(block);
    #else
        return 0;
    #endif
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

