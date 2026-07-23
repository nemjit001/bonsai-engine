#include "SlangShaderCompiler.hpp"

#include <Containers/DynamicArray.hpp>
#include <Containers/StaticArray.hpp>

namespace RHI
{
    SlangResult VirtualFileSystem::loadFile(char const* path, ISlangBlob** outBlob)
    {
        BONSAI_ASSERT(outBlob && "Out blob is nullptr!");
        String const contents = fs::ReadFile(path, nullptr);
        if (contents.empty()) {
            return -1;
        }

        *outBlob = slang_createBlob(contents.data(), contents.size());
        return SLANG_OK;
    }

    SlangShaderCompiler::SlangShaderCompiler(Slang::ComPtr<slang::IGlobalSession>&& globalSession)
        :
        _filesystem(new VirtualFileSystem()),
        _globalSession(globalSession),
        _activeSession(nullptr)
    {
        //
    }

    SlangShaderCompiler* SlangShaderCompiler::TryCreate()
    {
        // Create the global Slang session.
        Slang::ComPtr<slang::IGlobalSession> globalSession = nullptr;
        if (!SLANG_SUCCEEDED(slang::createGlobalSession(globalSession.writeRef()))) {
            return nullptr;
        }

        // Create the shader compiler and initialize the compiler with defaults.
        auto shaderCompiler = std::unique_ptr<SlangShaderCompiler>(new SlangShaderCompiler(std::move(globalSession)));
        if (!shaderCompiler->InitializeActiveSession()) {
            return nullptr;
        }

        return shaderCompiler.release();
    }

    void SlangShaderCompiler::SetTargetLanguage(ShaderLang targetLanguage)
    {
        _activeShaderLanguage = targetLanguage;
    }

    bool SlangShaderCompiler::CompileShaderSource(char const* entrypoint, char const* source, ShaderBlob* outBlob) const
    {
        BONSAI_ASSERT(_activeSession && "No active Slang session found!");
        BONSAI_ASSERT(source && "No shader source provided!");
        BONSAI_ASSERT(outBlob && "No output shader blob provided!");

        // Compile source string
        Slang::ComPtr<slang::IModule> compilationModule = nullptr;
        {
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            compilationModule = _activeSession->loadModuleFromSourceString(nullptr, nullptr, source, diagnostics.writeRef());

            TryPrintDiagnostics(diagnostics);
            if (!compilationModule) {
                return false;
            }
        }

        return CompileShaderModule(entrypoint, compilationModule, outBlob);
    }

    bool SlangShaderCompiler::CompileShaderFile(char const* entrypoint, fs::Path const& path, ShaderBlob* outBlob) const
    {
        BONSAI_ASSERT(_activeSession && "No active Slang session found!");
        BONSAI_ASSERT(outBlob && "No output shader blob provided!");

        // Get file path pointer
        // FIXME(nemjit001): Replace this std::string with a String type.
        std::string const filePath = path.string();

        // Read source file from disk
        Slang::ComPtr<slang::IBlob> source = nullptr;
        {
            if (SLANG_FAILED(_filesystem->loadFile(filePath.c_str(), source.writeRef()))) {
                return false;
            }
        }

        // Compile source file
        Slang::ComPtr<slang::IModule> compilationModule = nullptr;
        {
            // FIXME(nemjit001): Read source blob from disk.
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            compilationModule = _activeSession->loadModuleFromSource(
                nullptr,
                filePath.c_str(),
                source,
                diagnostics.writeRef()
            );

            TryPrintDiagnostics(diagnostics);
            if (!compilationModule) {
                return false;
            }
        }

        return CompileShaderModule(entrypoint, compilationModule, outBlob);
    }

    bool SlangShaderCompiler::InitializeActiveSession()
    {
        // Set up compilation targets
        slang::TargetDesc spirvTargetDesc{};
        spirvTargetDesc.format = SLANG_SPIRV;
        spirvTargetDesc.profile = _globalSession->findProfile("gl460");
        spirvTargetDesc.forceGLSLScalarBufferLayout = true;

        slang::TargetDesc dxilTargetDesc{};
        dxilTargetDesc.format = SLANG_DXIL;
        dxilTargetDesc.profile = _globalSession->findProfile("Shader Model 6.0");
        dxilTargetDesc.forceGLSLScalarBufferLayout = true;

        slang::TargetDesc wgslTargetDesc{};
        wgslTargetDesc.format = SLANG_WGSL;
        wgslTargetDesc.profile = SLANG_PROFILE_UNKNOWN;
        wgslTargetDesc.forceGLSLScalarBufferLayout = true;

        // Set up local compilation session
        // NOTE(nemjit001): This order is dependent on the ShaderLang enum
        StaticArray<slang::TargetDesc, 3> const targets = {
            spirvTargetDesc,    //< ShaderLang_SPIRV
            dxilTargetDesc,     //< ShaderLang_DXIL
            wgslTargetDesc,     //< ShaderLang_WGSL
        };

        // TODO(nemjit001): Set up search paths, preprocessor macros, etc.
        slang::SessionDesc sessionDesc{};
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_ROW_MAJOR;
        sessionDesc.fileSystem = _filesystem.get();
        sessionDesc.targetCount = targets.size();
        sessionDesc.targets = targets.data();
        sessionDesc.searchPathCount = 0;
        sessionDesc.searchPaths = nullptr;
        sessionDesc.preprocessorMacroCount = 0;
        sessionDesc.preprocessorMacros = nullptr;
        sessionDesc.compilerOptionEntryCount = 0;
        sessionDesc.compilerOptionEntries = nullptr;

        _activeSession = nullptr;
        return SLANG_SUCCEEDED(_globalSession->createSession(sessionDesc, _activeSession.writeRef()));
    }

    bool SlangShaderCompiler::CompileShaderModule(char const* entrypoint, Slang::ComPtr<slang::IModule> const& compilationModule, ShaderBlob* outBlob) const
    {
        // Query optional program entrypoint
        Slang::ComPtr<slang::IEntryPoint> compilationEntrypoint = nullptr;
        {
            if (entrypoint != nullptr)
            {
                compilationModule->findEntryPointByName(entrypoint, compilationEntrypoint.writeRef());
                if (!entrypoint)
                {
                    PrintDebugString("[RHIShaderCompiler]: Failed to find shader entrypoint (%s)", entrypoint);
                    return false;
                }
            }
        }

        // Compose program from module and optional entrypoint
        // FIXME(nemjit001): Use passed-in allocator for dyn array
        DynamicArray<slang::IComponentType*> programComponents;
        programComponents.reserve(2);
        programComponents.push_back(compilationModule);
        if (compilationEntrypoint) {
            programComponents.push_back(compilationEntrypoint);
        }

        Slang::ComPtr<slang::IComponentType> composedProgram = nullptr;
        {
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            SlangResult const result = _activeSession->createCompositeComponentType(
                programComponents.data(),
                programComponents.size(),
                composedProgram.writeRef(),
                diagnostics.writeRef()
            );

            TryPrintDiagnostics(diagnostics);
            if (SLANG_FAILED(result)) {
                return false;
            }
        }

        // Link program
        Slang::ComPtr<slang::IComponentType> linkedProgram = nullptr;
        {
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            SlangResult const result = composedProgram->link(
                linkedProgram.writeRef(),
                diagnostics.writeRef()
            );

            TryPrintDiagnostics(diagnostics);
            if (SLANG_FAILED(result)) {
                return false;
            }
        }

        // Compile shader code for linked program
        Slang::ComPtr<slang::IBlob> outShaderCode = nullptr;
        {
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            SlangResult const result = linkedProgram->getTargetCode(
                static_cast<SlangInt32>(_activeShaderLanguage),
                outShaderCode.writeRef(),
                diagnostics.writeRef()
            );

            TryPrintDiagnostics(diagnostics);
            if (SLANG_FAILED(result)) {
                return false;
            }
        }

        // TODO(nemjit001): Copy output shader code to shader blob
        return true;
    }

    void SlangShaderCompiler::TryPrintDiagnostics(Slang::ComPtr<slang::IBlob> const& diagnostics) const
    {
        if (diagnostics) {
            PrintDebugString("[RHIShaderCompiler]: %*.s", static_cast<char const*>(diagnostics->getBufferPointer()), diagnostics->getBufferSize());
        }
    }
} // namespace RHI

