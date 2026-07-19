#pragma once
#ifndef SLANG_SHADER_COMPILER_HPP
#define SLANG_SHADER_COMPILER_HPP

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>
#include "RHIShaderCompiler.hpp"

namespace RHI
{
    class SlangShaderCompiler : public IRHIShaderCompiler
    {
    private:
        SlangShaderCompiler(Slang::ComPtr<slang::IGlobalSession>&& globalSession);

    public:
        /// @brief Try to create a Slang shader compiler.
        /// @return A new Slang shader compiler, or nullptr on failure.
        static SlangShaderCompiler* TryCreate();

    private:
        Slang::ComPtr<slang::IGlobalSession> _globalSession = nullptr;
    };
} // namespace RHI

#endif //SLANG_SHADER_COMPILER_HPP
