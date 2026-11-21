#pragma once
#ifndef BONSAI_SHADER_COMPILER_HPP
#define BONSAI_SHADER_COMPILER_HPP

#include <slang.h>
#include <slang-com-ptr.h>
#include "Core/Filesystem.hpp"

namespace Bonsai
{
    /// @brief Enumeration of shader compile targets supported by the shader compiler.
    enum ShaderCompileTarget : uint32_t
    {
        ShaderCompileTarget_Vulkan13 = 0,
    };

    /// @brief The ShaderCompiler is used for runtime shader compilation.
    class ShaderCompiler
    {
    private:
        ShaderCompiler(
            Slang::ComPtr<slang::IGlobalSession> globalSession,
            Slang::ComPtr<slang::ISession> defaultSession
        );

    public:
        /// @brief Try to create a shader compiler.
        /// @return The newly created shader compiler, or nullptr on failure.
        [[nodiscard]] static ShaderCompiler* TryCreate();

        /// @brief Destroy a shader compiler.
        /// @param shaderCompiler Shader compiler to destroy.
        static void Destroy(ShaderCompiler* shaderCompiler);

        /// @brief Set the shader compile target for next compilations.
        /// @param compileTarget 
        void SetCompileTarget(ShaderCompileTarget compileTarget);

        /// @brief Compile a shader from a file path.
        /// @param path Path to the shader file.
        /// @return The compiled shader bytecode for the current compile target.
        Slang::ComPtr<slang::IBlob> CompileShaderFromFile(Filesystem::Path const& path);

    private:
        /// @brief Minimum supported shader model.
        static constexpr char const* MinShaderModel = "sm_6_2";

        ShaderCompileTarget _compileTarget = ShaderCompileTarget_Vulkan13;
        Slang::ComPtr<slang::IGlobalSession> _globalSession = nullptr;
        Slang::ComPtr<slang::ISession> _defaultSession = nullptr;
    };
} // namespace Bonsai

#endif //BONSAI_SHADER_COMPILER_HPP
