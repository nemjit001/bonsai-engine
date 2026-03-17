#include "ShaderCompiler.hpp"

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "Logging.hpp"

CComPtr<IDxcUtils> BShaderCompiler::Utils;
CComPtr<IDxcCompiler3> BShaderCompiler::Compiler;
CComPtr<IDxcIncludeHandler> BShaderCompiler::IncludeHandler;

ShaderCompilerResult BShaderCompiler::CompileFile(
    char const* FilePath,
    char const* Entrypoint,
    char const* TargetProfile,
    IDxcBlob** OutBlob
)
{
    if (!LazyInit()) {
        return SHADER_COMPILER_RESULT_BAD_INIT;
    }

    // Load shader file from disk
    std::wstring FileName = ToWideString(FilePath);
    BOOL EncodingKnown = FALSE;
    UINT32 CodePage = DXC_CP_ACP;
    CComPtr<IDxcBlobEncoding> SourceBlob;
    if (FAILED(Utils->LoadFile(FileName.c_str(), nullptr, &SourceBlob))
        || FAILED(SourceBlob->GetEncoding(&EncodingKnown, &CodePage))) {
        return SHADER_COMPILER_RESULT_COMPILATION_FAILED;
    }

    // Set up compiler args
    // TODO(nemjit001): Handle defines and preset compiler args
    std::wstring EntrypointName = ToWideString(Entrypoint);
    std::wstring TargetProfileName = ToWideString(TargetProfile);
    CComPtr<IDxcCompilerArgs> CompilerArgs;
    Utils->BuildArguments(
        FileName.c_str(),
        EntrypointName.c_str(),
        TargetProfileName.c_str(),
        nullptr, 0,
        nullptr, 0,
        &CompilerArgs
    );

    // Compile source file
    DxcBuffer SourceBuffer{};
    SourceBuffer.Ptr = SourceBlob->GetBufferPointer();
    SourceBuffer.Size = SourceBlob->GetBufferSize();
    SourceBuffer.Encoding = CodePage;

    return Compile(&SourceBuffer, CompilerArgs, OutBlob);
}

ShaderCompilerResult BShaderCompiler::CompileShaderSource(
    char const* ShaderSource,
    char const* Entrypoint,
    char const* TargetProfile,
    IDxcBlob** OutBlob
)
{
    if (!LazyInit()) {
        return SHADER_COMPILER_RESULT_BAD_INIT;
    }

    // Set up compiler args
    // TODO(nemjit001): Handle defines and preset compiler args
    std::wstring EntrypointName = ToWideString(Entrypoint);
    std::wstring TargetProfileName = ToWideString(TargetProfile);
    CComPtr<IDxcCompilerArgs> CompilerArgs;
    Utils->BuildArguments(
        nullptr,
        EntrypointName.c_str(),
        TargetProfileName.c_str(),
        nullptr, 0,
        nullptr, 0,
        &CompilerArgs
    );

    // Compile source file
    DxcBuffer SourceBuffer{};
    SourceBuffer.Ptr = ShaderSource;
    SourceBuffer.Size = std::strlen(ShaderSource);
    SourceBuffer.Encoding = DXC_CP_ACP;

    return Compile(&SourceBuffer, CompilerArgs, OutBlob);
}

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

std::wstring BShaderCompiler::ToWideString(std::string const& str)
{
    if (str.empty()) {
        return {};
    }

    size_t BufferLength = str.size() + 1;
    wchar_t* WideStringBuffer = new wchar_t[BufferLength]{};
    std::mbstowcs(WideStringBuffer, str.c_str(), BufferLength - 1);

    std::wstring WideString(WideStringBuffer, WideStringBuffer + BufferLength);
    delete[] WideStringBuffer;
    return WideString;
}

ShaderCompilerResult BShaderCompiler::Compile(
    DxcBuffer* SourceBuffer,
    IDxcCompilerArgs* CompilerArgs,
    IDxcBlob** OutBlob
)
{
    CComPtr<IDxcResult> CompileResult;
    Compiler->Compile(SourceBuffer, CompilerArgs->GetArguments(), CompilerArgs->GetCount(), IncludeHandler, IID_PPV_ARGS(&CompileResult));

    // Check compile status
    HRESULT CompileStatus;
    CompileResult->GetStatus(&CompileStatus);
    if (FAILED(CompileStatus))
    {
        CComPtr<IDxcBlobEncoding> ErrorBuffer;
        CompileResult->GetErrorBuffer(&ErrorBuffer);
        if (ErrorBuffer->GetBufferSize() > 0) {
            BONSAI_LOG(LogVerbosity::Error, "[Shader Compilation Error]:\n%s", static_cast<char const*>(ErrorBuffer->GetBufferPointer()));
        }

        return SHADER_COMPILER_RESULT_COMPILATION_FAILED;
    }

    CompileResult->GetResult(OutBlob);
    return SHADER_COMPILER_RESULT_OK;
}

#if __clang__
#pragma clang diagnostic pop
#endif
