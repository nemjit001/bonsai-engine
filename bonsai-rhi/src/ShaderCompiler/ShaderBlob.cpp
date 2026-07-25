#include "ShaderBlob.hpp"

namespace RHI
{
    ShaderBlob::ShaderBlob(void const* data, usize size)
    {
        // Allocate buffer
        _data = Memory::Malloc(size, Memory::DEFAULT_NEW_ALIGNMENT);
        _size = size;

        // Copy data to internal buffer
        Memory::MemCopy(_data, const_cast<void*>(data), size);
    }

    ShaderBlob::~ShaderBlob()
    {
        Memory::Free(_data);
    }
} // namespace RHI

