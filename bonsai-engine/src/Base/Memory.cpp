#include "Memory.hpp"

#include <cstdlib>
#include <cstring>
#include <new>
#include "Assert.hpp"
#include "PlatformDefines.hpp"

#if BONSAI_PLATFORM_LINUX
    #include <malloc.h>
#endif

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
    /// @brief Align a memory address to a byte boundary.
    /// @param address 
    /// @param alignment 
    /// @return 
    constexpr usize AlignAddress(uintptr address, usize alignment)
    {
        BONSAI_ASSERT(alignment > 0 && alignment % 2 == 0 && "Alignment should be non-zero and a multiple of 2!");
        return (address + alignment - 1) & ~(alignment - 1);
    }

    void* Malloc(usize size, usize alignment)
    {
        BONSAI_ASSERT(size > 0 && "Malloc size must be non-zero");

    #if     BONSAI_PLATFORM_WINDOWS
        return _aligned_malloc(size, alignment);
    #elif BONSAI_PLATFORM_LINUX
        return std::aligned_alloc(alignment, size);
    #else
        return nullptr;
    #endif
    }

    void* Malloc(usize size)
    {
        usize const alignment = alignof(std::max_align_t);
        usize const alignedSize = AlignAddress(size, alignment);
        return Malloc(alignedSize, alignment);
    }

    void* Calloc(usize count, usize size, usize alignment)
    {
        usize const blockSize = count * size;
        usize const alignedSize = AlignAddress(blockSize, alignment);
        void* block = Malloc(alignedSize, alignment);
        if (block == nullptr) {
            return nullptr;
        }

        MemSet(block, 0, alignedSize);
        return block;
    }

    void* Calloc(usize count, usize size)
    {
        return Calloc(count, size, alignof(std::max_align_t));
    }

    void* Realloc(void* block, usize size, usize alignment)
    {
    #if     BONSAI_PLATFORM_WINDOWS
        return _aligned_realloc(block, size, alignment);
    #elif   BONSAI_PLATFORM_LINUX
        // XXX(nemjit001): This is a simple pessimistic implementation of an aligned realloc since Linux does not provide this function in its standard lib.
        usize const oldSize = MallocSize(block);
        uintptr const address = reinterpret_cast<uintptr>(block);
        if (oldSize > size && AlignAddress(address, alignment) == address) {
            return block; // Reuse memory block if it is a shrinking resize and alignment matches
        }

        usize const copySize = oldSize < size ? oldSize : size; // Copy only available memory region
        void* newBlock = Malloc(size, alignment);
        MemCopy(newBlock, block, copySize);
        Free(block);
        return newBlock;
    #else
        return nullptr
    #endif
    }

    void* Realloc(void* block, usize size)
    {
        usize const alignment = alignof(std::max_align_t);
        usize const alignedSize = AlignAddress(size, alignment);
        return Realloc(block, alignedSize, alignment);
    }

    void Free(void* block)
    {
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
} // namespace Memory

