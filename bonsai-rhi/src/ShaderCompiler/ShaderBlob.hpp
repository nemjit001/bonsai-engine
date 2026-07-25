#pragma once
#ifndef SHADER_BLOB_HPP
#define SHADER_BLOB_HPP

#include "RHIShaderCompiler.hpp"

namespace RHI
{
    /// @brief Shader blob implementation.
    class ShaderBlob : public IShaderBlob
    {
    public:
        ShaderBlob(void const* data, usize size);
        ~ShaderBlob();

        ShaderBlob(ShaderBlob const&) = delete;
        ShaderBlob& operator=(ShaderBlob const&) = delete;

        virtual void const* GetBufferPointer() const override final { return _data; }
        virtual usize GetBufferSize() const override final { return _size; }

    private:
        void* _data = nullptr;
        usize _size = 0;
    };
} // namespace RHI


#endif //SHADER_BLOB_HPP
