#pragma once
#ifndef RHI_SHADER_COMPILER_HPP
#define RHI_SHADER_COMPILER_HPP

#include "BonsaiRHIExport.hpp"

namespace RHI
{
    struct ShaderBlob;

    /// @brief The RHIShaderCompiler interface implements an abstraction on top of low-level shader compilation for RHI backends.
    class BONSAI_RHI_EXPORT IRHIShaderCompiler
    {
    public:
        virtual ~IRHIShaderCompiler() = default;

        [[nodiscard]] virtual bool CompileShaderSource(char const* entrypoint, char const* source, ShaderBlob* outBlob) const = 0;
    };

    /// @brief Create an RHI shader compiler.
    /// @return A newly created shader compiler, or nullptr on failure.
    [[nodiscard]] BONSAI_RHI_EXPORT IRHIShaderCompiler* BONSAI_RHI_APICALL CreateShaderCompiler();

    /// @brief Destroy a shader compiler.
    /// @param shaderCompiler Shader compiler to destroy.
    BONSAI_RHI_EXPORT void BONSAI_RHI_APICALL DestroyShaderCompiler(IRHIShaderCompiler* shaderCompiler);
} // namespace RHI

#endif //RHI_SHADER_COMPILER_HPP
