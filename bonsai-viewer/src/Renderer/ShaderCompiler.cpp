#include "ShaderCompiler.hpp"

#include <array>
#include <string>
#include <vector>
#include "Logging.hpp"

CComPtr<IDxcUtils> BShaderCompiler::Utils;
CComPtr<IDxcCompiler3> BShaderCompiler::Compiler;

/// @brief Default DXC compilation arguments.
static constexpr std::array DefaultDxcArguments = {
    DXC_ARG_DEBUG,                  // Include debug info
    DXC_ARG_OPTIMIZATION_LEVEL3,    // Enable highest optimization level
    DXC_ARG_PACK_MATRIX_ROW_MAJOR,  // Use row-major matrix order
};

/// @brief SPIRV compilation arguments, set up for Vulkan specific compilation.
static constexpr std::array SPIRVDxcArguments = {
    L"-spirv",
    L"-fspv-target-env=vulkan1.3",
};

ShaderCompileResult BShaderCompiler::Compile(ShaderLang InputLanguage, CompilerOutput OutputType, LPCWSTR TargetProfile, BShaderSource const& ShaderSource, IDxcBlob** OutBlob)
{
    // Initialize DXC API
    if (!Utils || !Compiler)
    {
        if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Utils)))
            || FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler)))
        ) {
            return SHADER_COMPILE_RESULT_BAD_INIT;
        }
    }
    
    // Translate params to wchar strings
    std::wstring const ShaderEntrypoint(ShaderSource.Entrypoint, ShaderSource.Entrypoint + std::strlen(ShaderSource.Entrypoint) + 1);

    // Set up compiler args
    std::vector<LPCWSTR> BaseArgs(std::begin(DefaultDxcArguments), std::end(DefaultDxcArguments));
    if (OutputType == CompilerOutput_SPIRV) {
        BaseArgs.insert(std::end(BaseArgs), std::begin(SPIRVDxcArguments), std::end(SPIRVDxcArguments));
    }

    CComPtr<IDxcCompilerArgs> CompilerArgs;
    Utils->BuildArguments(
        nullptr,
        ShaderEntrypoint.c_str(),
        TargetProfile,
        BaseArgs.data(), static_cast<uint32_t>(BaseArgs.size()),
        nullptr, 0,
        &CompilerArgs
    );

    // Compile shader
    DxcBuffer SourceBuffer{};
    SourceBuffer.Ptr = ShaderSource.Source;
    SourceBuffer.Size = ShaderSource.SourceLength;
    SourceBuffer.Encoding = 0; // Autodetect input encoding

    CComPtr<IDxcResult> CompileResult;
    if (FAILED(Compiler->Compile(&SourceBuffer, CompilerArgs->GetArguments(), CompilerArgs->GetCount(), nullptr, IID_PPV_ARGS(&CompileResult)))
        || FAILED(CompileResult->GetResult(OutBlob))) {
        return SHADER_COMPILE_RESULT_COMPILATION_FAILED;
    }

    // Check compile status and dump error buffer
    HRESULT CompileStatus;
    CompileResult->GetStatus(&CompileStatus);
    if (FAILED(CompileStatus))
    {
        CComPtr<IDxcBlobEncoding> CompilationErrors;
        CompileResult->GetErrorBuffer(&CompilationErrors);
        if (CompilationErrors) {
            BONSAI_LOG(LogVerbosity::Verbose, "ShaderCompiler:\n%s", static_cast<char const*>(CompilationErrors->GetBufferPointer()));
        }

        return SHADER_COMPILE_RESULT_COMPILATION_FAILED;
    }

    return SHADER_COMPILE_RESULT_OK;
}
