#pragma once
#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "PlatformTypes.hpp"

namespace Memory
{
    static constexpr usize DEFAULT_NEW_ALIGNMENT = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

    /// @brief Allocate a block of memory.
    /// @param size Size of the memory block.
    /// @param alignment Alignment in bytes for the memory block.
    /// @return A pointer to the memory block, or nullptr on failure.
    [[nodiscard]] void* Malloc(usize size, usize alignment);
    [[nodiscard]] void* Malloc(usize size);

    /// @brief Allocate a contiguous block of zeroed memory.
    /// @param count Number of entries in the array.
    /// @param size Size of the array entry.
    /// @param alignment Alignment in bytes for the start of the memory block.
    /// @return A pointer to the memory blcok, or nullptr on failure.
    [[nodiscard]] void* Calloc(usize count, usize size, usize alignment);
    [[nodiscard]] void* Calloc(usize count, usize size);

    /// @brief Reallocate a block of memory.
    /// @param block Original pointer to the memory block.
    /// @param size New size of the memory block.
    /// @param alignment Alignment in bytes for the memory block.
    /// @return A pointer to the resized memory block, or nullptr on failure.
    [[nodiscard]] void* Realloc(void* block, usize size, usize alignment);
    [[nodiscard]] void* Realloc(void* block, usize size);

    /// @brief Free a block of memory.
    /// @param block Pointer to the memory block to free.
    void Free(void* block);

    /// @brief Get the usable size of an allocated memory block.
    /// @param block Pointer to the memory block to query.
    /// @return The size of the memory block.
    usize UsableSize(void* block);

    /// @brief Set a block of memory to a value.
    /// @param block Pointer to the destination memory block.
    /// @param value Value to set.
    /// @param size Size of the memory block.
    void MemSet(void* block, int value, usize size);

    /// @brief Copy a memory region from one block to another.
    /// @param dest Destination block.
    /// @param src Source block.
    /// @param size Size in bytes to copy.
    void MemCopy(void* dest, void* src, usize size);

    /// @brief Move a memory region from one block to another. Overlapping regions are handled correctly.
    /// @param dest Destination block.
    /// @param src Source block.
    /// @param size Size in bytes to copy.
    void MemMove(void* dest, void* src, usize size);
}; // namespace Memory

#endif //MEMORY_HPP
