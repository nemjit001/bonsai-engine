#include "SlangShaderCompiler.hpp"

#include <BonsaiBase.hpp>
#include <Containers/DynamicArray.hpp>

namespace RHI
{
    SlangShaderCompiler::SlangShaderCompiler(Slang::ComPtr<slang::IGlobalSession>&& globalSession)
        :
        _globalSession(globalSession)
    {
        //
    }

    SlangShaderCompiler* SlangShaderCompiler::TryCreate()
    {
        Slang::ComPtr<slang::IGlobalSession> globalSession = nullptr;
        if (!SLANG_SUCCEEDED(slang::createGlobalSession(globalSession.writeRef()))) {
            return nullptr;
        }

        return new SlangShaderCompiler(std::move(globalSession));
    }

    bool SlangShaderCompiler::CompileShaderSource(char const* entrypoint, char const* source, ShaderBlob* outBlob) const
    {
        BONSAI_ASSERT(source && "No shader source provided!");
        BONSAI_ASSERT(outBlob && "No output shader blob provided!");

        // Set up local compilation session
        // TODO(nemjit001): Set targets, search paths, preprocessor macros, etc.
        slang::SessionDesc sessionDesc{};
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_ROW_MAJOR;
        sessionDesc.fileSystem = nullptr;
        sessionDesc.targets = 0;
        sessionDesc.targets = nullptr;
        sessionDesc.searchPathCount = 0;
        sessionDesc.searchPaths = nullptr;
        sessionDesc.preprocessorMacroCount = 0;
        sessionDesc.preprocessorMacros = nullptr;
        sessionDesc.compilerOptionEntryCount = 0;
        sessionDesc.compilerOptionEntries = nullptr;

        Slang::ComPtr<slang::ISession> session = nullptr;
        if (SLANG_FAILED(_globalSession->createSession(sessionDesc, session.writeRef()))) {
            return false;
        }

        // Compile source string
        Slang::ComPtr<slang::IModule> compilationModule = nullptr;
        {
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            compilationModule = session->loadModuleFromSourceString(nullptr, nullptr, source, diagnostics.writeRef());
            TryPrintDiagnostics(diagnostics);

            if (!compilationModule) {
                return false;
            }
        }

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
            SlangResult const result = session->createCompositeComponentType(programComponents.data(), programComponents.size(), composedProgram.writeRef(), diagnostics.writeRef());
            TryPrintDiagnostics(diagnostics);

            if (SLANG_FAILED(result)) {
                return false;
            }
        }

        // Link program
        Slang::ComPtr<slang::IComponentType> linkedProgram = nullptr;
        {
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            SlangResult const result = composedProgram->link(linkedProgram.writeRef(), diagnostics.writeRef());
            TryPrintDiagnostics(diagnostics);

            if (SLANG_FAILED(result)) {
                return false;
            }
        }

        // Compile shader code for linked program
        Slang::ComPtr<slang::IBlob> outShaderCode = nullptr;
        {
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            SlangResult const result = linkedProgram->getEntryPointCode(0 /* entrypoint index */, 0 /* target index */, outShaderCode.writeRef(), diagnostics.writeRef());
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

