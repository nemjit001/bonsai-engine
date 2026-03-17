#include <gtest/gtest.h>
#include "Renderer/ShaderCompiler.hpp"

static constexpr char const* ComputeShaderSource = R"(
[shader("compute")]
[numthreads(1, 1, 1)]
void CSMain()
{
    // Nothing to see here...
}
)";

TEST(ShaderCompilerTests, TestCompileShaderSource)
{
    BShaderCompilerArgs CompilerArgs{};
    CompilerArgs.IncludePaths = nullptr;
    CompilerArgs.IncludePathCount = 0;
    CompilerArgs.Defines = nullptr;
    CompilerArgs.DefineCount = 0;

    CComPtr<IDxcBlob> Output;
    ASSERT_EQ(BShaderCompiler::CompileShaderSource(ComputeShaderSource, "CSMain", "cs_6_0", CompilerArgs, &Output), SHADER_COMPILER_RESULT_OK);
}
