#include "Memory.hpp"

#include <cstdlib>
#include <cstring>
#include "Assert.hpp"
#include "PlatformDefines.hpp"

#if BONSAI_PLATFORM_LINUX
    #include <malloc.h>
#endif

namespace Memory
{
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
        return std::malloc(size);
    }

    void* Calloc(usize count, usize size, usize alignment)
    {
        usize const blockSize = count * size;
        void* block = Malloc(blockSize, alignment);
        if (block == nullptr) {
            return nullptr;
        }

        MemSet(block, 0, blockSize);
        return block;
    }

    void* Calloc(usize count, usize size)
    {
        return std::calloc(count, size);
    }

    void* Realloc(void* block, usize size, usize alignment)
    {
    #if     BONSAI_PLATFORM_WINDOWS
        return _aligned_realloc(block, size, alignment);
    #elif   BONSAI_PLATFORM_LINUX
        // XXX(nemjit001): This is a simple pessimistic implementation since Linux does not provide an aligned realloc function.
        usize const oldSize = MallocSize(block);
        void* newBlock = Malloc(size, alignment);
        MemCopy(newBlock, block, oldSize);
        Free(block);
        return newBlock;
    #else
        return nullptr
    #endif
    }

    void* Realloc(void* block, usize size)
    {
        return std::realloc(block, size);
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

