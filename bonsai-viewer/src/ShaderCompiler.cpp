#include "ShaderCompiler.hpp"

#include <array>
#include <string>
#include <vector>
#include "Logging.hpp"

CComPtr<IDxcUtils> BShaderCompiler::Utils;
CComPtr<IDxcCompiler3> BShaderCompiler::Compiler;
CComPtr<IDxcIncludeHandler> BShaderCompiler::IncludeHandler;

bool BShaderCompiler::LazyInit()
{
    if (!Utils || !Compiler || !IncludeHandler)
    {
        if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Utils)))
            || FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler)))
            || FAILED(Utils->CreateDefaultIncludeHandler(&IncludeHandler))
        ) {
            return false;
        }
    }

    return true;
}

ShaderCompilerResult BShaderCompiler::PreprocessShaderSource(
    char const* ShaderSource,
    size_t ShaderSourceLength,
    char const** IncludeDirs,
    size_t IncludeDirCount,
    IDxcBlob** OutBlob
)
{
    if (!LazyInit()) {
        return SHADER_COMPILER_RESULT_BAD_INIT;
    }

    // Build list of include dirs
    std::vector<std::wstring> WideIncludeDirs;
    WideIncludeDirs.reserve(IncludeDirCount);
    for (size_t i = 0; i < IncludeDirCount; i++)
    {
        std::wstring const WideDir(IncludeDirs[i], IncludeDirs[i] + std::strlen(IncludeDirs[i]) + 1);
        WideIncludeDirs.emplace_back(L"-I " + WideDir);
    }

    // Create compiler args
    std::array PreprocessArgs = {
        L"-P" // Preprocess file only
    };

    CComPtr<IDxcCompilerArgs> CompilerArgs;
    Utils->BuildArguments(nullptr, nullptr, nullptr,
        PreprocessArgs.data(), PreprocessArgs.size(),
        nullptr, 0, // TODO(nemjit001): Check if resolving user defines could also be done here
        &CompilerArgs
    );

    // Add include paths to args to resolve include directives
    for (auto const& IncludeDir : WideIncludeDirs) {
        LPCWSTR IncludeDirStr = IncludeDir.c_str();
        CompilerArgs->AddArguments(&IncludeDirStr, 1);
    }

    // Preprocess shader source through compiler infrastructure
    DxcBuffer Source{};
    Source.Ptr = ShaderSource;
    Source.Size = ShaderSourceLength;
    Source.Encoding = 0;

    CComPtr<IDxcResult> CompileResult;
    if (FAILED(Compiler->Compile(&Source, CompilerArgs->GetArguments(), CompilerArgs->GetCount(), IncludeHandler, IID_PPV_ARGS(&CompileResult)))) {
        return SHADER_COMPILER_RESULT_PREPROCESS_FAILED;
    }

    // Check compile status and dump any compilation errors
    HRESULT CompileStatus;
    CompileResult->GetStatus(&CompileStatus);
    if (FAILED(CompileStatus))
    {
        CComPtr<IDxcBlobEncoding> ErrorBlob;
        CompileResult->GetErrorBuffer(&ErrorBlob);
        if (ErrorBlob->GetBufferSize() > 0) {
            BONSAI_LOG(LogVerbosity::Error, "Shader preprocess error:\n%s", static_cast<char const*>(ErrorBlob->GetBufferPointer()));
        }

        return SHADER_COMPILER_RESULT_PREPROCESS_FAILED;
    }

    CompileResult->GetOutput(DXC_OUT_HLSL, IID_PPV_ARGS(OutBlob), nullptr);
    return SHADER_COMPILER_RESULT_OK;
}

ShaderCompilerResult BShaderCompiler::CompileShaderSource(
    char const* ShaderSource,
    size_t ShaderSourceLength,
    char const** Arguments,
    size_t ArgumentCount,
    IDxcBlob** OutBlob
)
{
    return SHADER_COMPILER_RESULT_COMPILATION_FAILED;
}
