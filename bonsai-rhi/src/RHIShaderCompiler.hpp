#pragma once
#ifndef RHI_SHADER_COMPILER_HPP
#define RHI_SHADER_COMPILER_HPP

#include <BonsaiBase.hpp>
#include "BonsaiRHIExport.hpp"

namespace RHI
{
    struct ShaderBlob;

    /// @brief Available shader language compilation targets.
    enum ShaderLang : int32
    {
        ShaderLang_SPIRV    = 0,
        ShaderLang_DXIL     = 1,
        ShaderLang_WGSL     = 2,
    };

    /// @brief The RHIShaderCompiler interface implements an abstraction on top of low-level shader compilation for RHI backends.
    class BONSAI_RHI_EXPORT IRHIShaderCompiler
    {
    public:
        virtual ~IRHIShaderCompiler() = default;

        /// @brief Compile a shader source string into an intermediate shader language representation.
        /// @param targetLanguage Target language for compilation.
        /// @param entrypoint Shader entrypoint name.
        /// @param source Shader source string.
        /// @param outBlob Output shader blob.
        /// @return A boolean indicating successful compilation.
        [[nodiscard]] virtual bool CompileShaderSource(ShaderLang targetLanguage, char const* entrypoint, char const* source, ShaderBlob* outBlob) const = 0;
    };

    /// @brief Create an RHI shader compiler.
    /// @return A newly created shader compiler, or nullptr on failure.
    [[nodiscard]] BONSAI_RHI_EXPORT IRHIShaderCompiler* BONSAI_RHI_APICALL CreateShaderCompiler();

    /// @brief Destroy a shader compiler.
    /// @param shaderCompiler Shader compiler to destroy.
    BONSAI_RHI_EXPORT void BONSAI_RHI_APICALL DestroyShaderCompiler(IRHIShaderCompiler* shaderCompiler);
} // namespace RHI

#endif //RHI_SHADER_COMPILER_HPP
