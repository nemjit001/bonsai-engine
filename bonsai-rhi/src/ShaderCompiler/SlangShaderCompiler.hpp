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

        virtual void SetTargetLanguage(ShaderLang targetLanguage) override final;
        virtual bool CompileShaderSource(char const* entrypoint, char const* source, ShaderBlob* outBlob) const override final;
        virtual bool CompileShaderFile(char const* entrypoint, fs::Path const& path, ShaderBlob* outBlob) const override final;

    private:
        /// @brief Initialize the active Slang session.
        /// @return A boolean indicating successful session initialization.
        [[nodiscard]] bool InitializeActiveSession();

        /// @brief Compile a Slang shader module into IR.
        /// @param entrypoint Entrypoint name.
        /// @param compilationModule Module to compile.
        /// @param outBlob Output shader blob.
        /// @return A boolean indicating successful compilation.
        [[nodiscard]] bool CompileShaderModule(char const* entrypoint, Slang::ComPtr<slang::IModule> const& compilationModule, ShaderBlob* outBlob) const;

        /// @brief Try to print diagnostics if the blob exists.
        /// @param diagnostics Diagnostics blob to print.
        void TryPrintDiagnostics(Slang::ComPtr<slang::IBlob> const& diagnostics) const;

    private:
        ShaderLang _activeShaderLanguage = ShaderLang_SPIRV;
        Slang::ComPtr<slang::IGlobalSession> _globalSession = nullptr;
        Slang::ComPtr<slang::ISession> _activeSession = nullptr;
    };
} // namespace RHI

#endif //SLANG_SHADER_COMPILER_HPP
