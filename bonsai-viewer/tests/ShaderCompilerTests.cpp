#include <gtest/gtest.h>
#include "ShaderCompiler.hpp"

TEST(ShaderCompilerTests, TestCompileComputeShader)
{
    constexpr char const* ComputeShader = R"(
[shader("compute")]
[numthreads(1, 1, 1)]
void CSMain()
{
}
)";

    CComPtr<IDxcBlob> CompiledShader{};
    ASSERT_EQ(BShaderCompiler::Compile(ShaderLang_HLSL, CompilerOutput_SPIRV, TARGET_PROFILE_COMPUTE_SHADER("6_0"), "CSMain", ComputeShader, &CompiledShader), SHADER_COMPILE_RESULT_OK);
}
