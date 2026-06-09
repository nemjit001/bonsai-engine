#pragma once
#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <memory_resource>
#include "PlatformTypes.hpp"

template<typename Type, typename ...Args>
constexpr Type* Construct(void* ptr, Args&&... args)
{
    return new (ptr) Type(std::forward<Args>(args)...);
}

template<typename Type>
constexpr Type* ConstructArray(void* ptr, usize count)
{
    return new (ptr) Type[count]{};
}

template<typename Type>
constexpr void Destruct(Type* ptr)
{
    ptr->~Type();
}

template<typename Type>
constexpr void DestructArray(Type* ptr, usize count)
{
    for (usize i = 0; i < count; i++) {
        Destruct(&ptr[i]);
    }
}

/// @brief The Allocator interface provides a unified way of allocating memory.
/// NOTE(nemjit001): By inheriting std::pmr::memory_resource this allocator interface also works with std::pmr::* types.
class IAllocator : public std::pmr::memory_resource
{
public:
    virtual ~IAllocator() = default;

    /// @brief Allocate a block of memory.
    /// @param size Size of the memory block in bytes.
    /// @param alignment Alignment for the memory block in bytes.
    /// @return The newly allocated memory block.
    virtual void* Allocate(usize size, usize alignment) = 0;

    /// @brief Deallocate a memory block.
    /// @param ptr Pointer to the block to deallocate.
    virtual void Deallocate(void* ptr) = 0;

private:
    void* do_allocate(std::size_t size, std::size_t alignment) override { return Allocate(size, alignment); }
    void do_deallocate(void* ptr, [[maybe_unused]] std::size_t bytes, [[maybe_unused]] std::size_t alignment) override { return Deallocate(ptr); }
    bool do_is_equal(std::pmr::memory_resource const& other) const noexcept override { return this == &other; }
};

class DefaultAllocator : public IAllocator
{
public:
    void* Allocate(usize size, usize alignment) override;

    void Deallocate(void* ptr) override;
};

#endif //ALLOCATOR_HPP
