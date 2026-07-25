#pragma once
#ifndef SLANG_SHADER_COMPILER_HPP
#define SLANG_SHADER_COMPILER_HPP

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>
#include "RHIShaderCompiler.hpp"

namespace RHI
{
    /// @brief Virtual filesystem for the shader compiler.
    class VirtualFileSystem : public ISlangFileSystem
    {
    public:
        SLANG_NO_THROW SlangResult SLANG_MCALL loadFile(char const* path, ISlangBlob** outBlob) override final;

        SLANG_NO_THROW uint32_t SLANG_MCALL addRef() override final
        {
            return _refcount.fetch_add(1, std::memory_order_acq_rel);
        };

        SLANG_NO_THROW uint32_t SLANG_MCALL release() override final
        {
            return _refcount.fetch_sub(1, std::memory_order_acq_rel);
        };

        SLANG_NO_THROW SlangResult SLANG_MCALL queryInterface(SlangUUID const&, void**) override final
        {
            return -1; // Always fails
        };

        SLANG_NO_THROW void* SLANG_MCALL castAs(const SlangUUID&) override final
        {
            return nullptr; // Always fails
        };

    private:
        std::atomic<uint32_t> _refcount = 0;
    };

    /// @brief The SlangShaderCompiler is an implementation of the RHI shader compiler interface.
    class SlangShaderCompiler : public IRHIShaderCompiler
    {
    private:
        SlangShaderCompiler(Slang::ComPtr<slang::IGlobalSession>&& globalSession);

    public:
        /// @brief Try to create a Slang shader compiler.
        /// @return A new Slang shader compiler, or nullptr on failure.
        static SlangShaderCompiler* TryCreate();

        virtual void SetTargetLanguage(ShaderLang targetLanguage) override final;
        virtual bool CompileShaderSource(char const* entrypoint, char const* source, IShaderBlob** outBlob) override final;
        virtual bool CompileShaderFile(char const* entrypoint, fs::Path const& path, IShaderBlob** outBlob) override final;

    private:
        /// @brief Initialize the active Slang session.
        /// @return A boolean indicating successful session initialization.
        [[nodiscard]] bool InitializeActiveSession();

        /// @brief Compile a Slang shader module into IR.
        /// @param entrypoint Entrypoint name.
        /// @param compilationModule Module to compile.
        /// @param outBlob Output shader blob.
        /// @return A boolean indicating successful compilation.
        [[nodiscard]] bool CompileShaderModule(char const* entrypoint, Slang::ComPtr<slang::IModule> const& compilationModule, IShaderBlob** outBlob) const;

        /// @brief Try to print diagnostics if the blob exists.
        /// @param diagnostics Diagnostics blob to print.
        void TryPrintDiagnostics(Slang::ComPtr<slang::IBlob> const& diagnostics) const;

    private:
        VirtualFileSystem _filesystem = VirtualFileSystem();
        ShaderLang _activeShaderLanguage = ShaderLang_SPIRV;
        Slang::ComPtr<slang::IGlobalSession> _globalSession = nullptr;
        Slang::ComPtr<slang::ISession> _activeSession = nullptr;
    };
} // namespace RHI

#endif //SLANG_SHADER_COMPILER_HPP
