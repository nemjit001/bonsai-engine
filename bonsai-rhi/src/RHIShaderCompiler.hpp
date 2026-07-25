#pragma once
#ifndef RHI_SHADER_COMPILER_HPP
#define RHI_SHADER_COMPILER_HPP

#include <BonsaiBase.hpp>
#include "BonsaiRHIExport.hpp"

namespace RHI
{
    /// @brief Available shader language compilation targets.
    enum ShaderLang : int32
    {
        ShaderLang_SPIRV    = 0,
        ShaderLang_DXIL     = 1,
        ShaderLang_WGSL     = 2,
    };

    /// @brief The ShaderBlob interface provides an interface for using binary blob data.
    class BONSAI_RHI_EXPORT IShaderBlob
    {
    public:
        virtual ~IShaderBlob() = default;

        /// @brief Get the stored buffer pointer.
        /// @return Data pointer for the buffer.
        virtual void const* GetBufferPointer() const = 0;

        /// @brief Get the stored buffer size.
        /// @return Size of the buffer.
        virtual usize GetBufferSize() const = 0;
    };

    /// @brief The RHIShaderCompiler interface implements an abstraction on top of low-level shader compilation for RHI backends.
    class BONSAI_RHI_EXPORT IRHIShaderCompiler
    {
    public:
        virtual ~IRHIShaderCompiler() = default;

        /// @brief Set the target shader language for compilation.
        /// @param targetLanguage Target language for compilation.
        virtual void SetTargetLanguage(ShaderLang targetLanguage) = 0;

        /// @brief Compile a shader source string into an intermediate shader language representation.
        /// @param entrypoint Optional shader entrypoint name, may be null.
        /// @param source Shader source string.
        /// @param outBlob Output shader blob.
        /// @return A boolean indicating successful compilation.
        [[nodiscard]] virtual bool CompileShaderSource(char const* entrypoint, char const* source, IShaderBlob** outBlob) = 0;

        /// @brief Compile a shader file from disk into an intermediate shader language representation.
        /// @param entrypoint Optional shader entrypoint name, may be null.
        /// @param path Shader file path, can be relative or absolute.
        /// @param outBlob Output shader blob.
        /// @return A booleain indicating successful compilation.
        [[nodiscard]] virtual bool CompileShaderFile(char const* entrypoint, fs::Path const& path, IShaderBlob** outBlob) = 0;
    };

    /// @brief Create an RHI shader compiler.
    /// @return A newly created shader compiler, or nullptr on failure.
    [[nodiscard]] BONSAI_RHI_EXPORT IRHIShaderCompiler* BONSAI_RHI_APICALL CreateShaderCompiler();

    /// @brief Destroy a shader compiler.
    /// @param shaderCompiler Shader compiler to destroy.
    BONSAI_RHI_EXPORT void BONSAI_RHI_APICALL DestroyShaderCompiler(IRHIShaderCompiler* shaderCompiler);

    /// @brief Create a shader blob.
    /// @param data Blob data pointer.
    /// @param size Blob data size.
    /// @return A new shader blob.
    [[nodiscard]] BONSAI_RHI_EXPORT IShaderBlob* BONSAI_RHI_APICALL CreateShaderBlob(void const* data, usize size);

    /// @brief Destroy a shader blob.
    /// @param blob Shader blob to destroy.
    BONSAI_RHI_EXPORT void BONSAI_RHI_APICALL DestroyShaderBlob(IShaderBlob* blob);
} // namespace RHI

#endif //RHI_SHADER_COMPILER_HPP
