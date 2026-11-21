#include "ShaderCompiler.hpp"

#include <Core/Logger.hpp>

namespace Bonsai
{
    /// @brief Dump slang diagnostics.
    /// @param diagnostics Diagnostics to dump.
    static void DumpDiagnostics(Slang::ComPtr<slang::IBlob> const& diagnostics)
    {
        if (diagnostics == nullptr) {
            return;
        }

        SPDLOG_WARN("Slang diagnostics: {}", reinterpret_cast<char const*>(diagnostics->getBufferPointer()));
    }

    ShaderCompiler::ShaderCompiler(
        Slang::ComPtr<slang::IGlobalSession> globalSession,
        Slang::ComPtr<slang::ISession> defaultSession
    )
        :
        _globalSession(globalSession),
        _defaultSession(defaultSession)
    {
        //
    }

    ShaderCompiler* ShaderCompiler::TryCreate()
    {
        // Create global session
        Slang::ComPtr<slang::IGlobalSession> globalSession{};
        if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef()))) {
            return nullptr;
        }

        // Set up targets
        slang::TargetDesc vulkan13Target{};
        vulkan13Target.format = SLANG_SPIRV;
        vulkan13Target.profile = globalSession->findProfile(MinShaderModel);
        vulkan13Target.compilerOptionEntryCount = 0;
        vulkan13Target.compilerOptionEntries = nullptr;

        slang::TargetDesc targets[] = {
            vulkan13Target,
        };

        // Create default session
        slang::SessionDesc defaultSessionDesc{};
        defaultSessionDesc.targetCount = std::size(targets);
        defaultSessionDesc.targets = targets;
        defaultSessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_ROW_MAJOR;
        defaultSessionDesc.fileSystem = nullptr;
        defaultSessionDesc.compilerOptionEntryCount = 0;
        defaultSessionDesc.compilerOptionEntries = nullptr;

        Slang::ComPtr<slang::ISession> defaultSession{};
        if (SLANG_FAILED(globalSession->createSession(defaultSessionDesc, defaultSession.writeRef()))) {
            return nullptr;
        }

        return new ShaderCompiler(globalSession, defaultSession);
    }

    void ShaderCompiler::Destroy(ShaderCompiler* shaderCompiler)
    {
        delete shaderCompiler;
    }

    void ShaderCompiler::SetCompileTarget(ShaderCompileTarget compileTarget)
    {
        _compileTarget = compileTarget;
    }

    Slang::ComPtr<slang::IBlob> ShaderCompiler::CompileShaderFromFile(Filesystem::Path const& path)
    {
        // Create shader module
        std::string const pathString = path.string();
        Slang::ComPtr<slang::IModule> shaderModule{};
        {
            Slang::ComPtr<slang::IBlob> diagnostics{};
            shaderModule = _defaultSession->loadModuleFromSource(
                nullptr,
                pathString.c_str(),
                nullptr,
                diagnostics.writeRef()
            );

            DumpDiagnostics(diagnostics);
            if (shaderModule == nullptr) {
                return nullptr;
            }
        }

        // Create shader program
        Slang::ComPtr<slang::IComponentType> shaderProgram{};
        {
            slang::IComponentType* componentTypes[] = {
                shaderModule,
            };

            Slang::ComPtr<slang::IBlob> diagnostics{};
            SlangResult const result = _defaultSession->createCompositeComponentType(
                componentTypes,
                std::size(componentTypes),
                shaderProgram.writeRef(),
                diagnostics.writeRef()
            );
            
            DumpDiagnostics(diagnostics);
            if (SLANG_FAILED(result)) {
                return nullptr;
            }
        }

        // Link shader program
        Slang::ComPtr<slang::IComponentType> linkedProgram{};
        {
            Slang::ComPtr<slang::IBlob> diagnostics{};
            SlangResult const result = shaderProgram->link(
                linkedProgram.writeRef(),
                diagnostics.writeRef()
            );

            DumpDiagnostics(diagnostics);
            if (SLANG_FAILED(result)) {
                return nullptr;
            }
        }

        // Compiled shader program
        Slang::ComPtr<slang::IBlob> shaderCode{};
        {
            Slang::ComPtr<slang::IBlob> diagnostics{};
            SlangResult const result = linkedProgram->getTargetCode(
                static_cast<SlangInt>(_compileTarget),
                shaderCode.writeRef(),
                diagnostics.writeRef()
            );

            DumpDiagnostics(diagnostics);
            if (SLANG_FAILED(result)) {
                return nullptr;
            }
        }

        return shaderCode;
    }
} // namespace Bonsai
