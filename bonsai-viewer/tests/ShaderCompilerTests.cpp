#include <gtest/gtest.h>
#include "ShaderCompiler.hpp"

static constexpr char const* ComputeShaderSource = R"(
[shader("compute")]
[numthreads(1, 1, 1)]
void CSMain()
{
    // Nothing to see here...
}
)";

TEST(ShaderCompilerTests, TestPreprocessComputeShader)
{
    CComPtr<IDxcBlob> PreprocessOutput;
    ShaderCompilerResult Result = BShaderCompiler::PreprocessShaderSource(
        ComputeShaderSource, sizeof(ComputeShaderSource),
        nullptr, 0, // No include directories
        &PreprocessOutput
    );
    ASSERT_EQ(Result, SHADER_COMPILER_RESULT_OK);
}

TEST(ShaderCompilerTests, TestCompileComputeShader)
{
    CComPtr<IDxcBlob> PreprocessOutput;
    ShaderCompilerResult PreprocessResult = BShaderCompiler::PreprocessShaderSource(
        ComputeShaderSource, sizeof(ComputeShaderSource),
        nullptr, 0, // No include directories
        &PreprocessOutput
    );
    ASSERT_EQ(PreprocessResult, SHADER_COMPILER_RESULT_OK);

    CComPtr<IDxcBlob> CompileOutput;
    ShaderCompilerResult CompileResult = BShaderCompiler::CompileShaderSource(
        static_cast<char const*>(PreprocessOutput->GetBufferPointer()), PreprocessOutput->GetBufferSize(),
        nullptr, 0, // No additional CLI args
        &CompileOutput
    );
    ASSERT_EQ(CompileResult, SHADER_COMPILER_RESULT_OK);
}
