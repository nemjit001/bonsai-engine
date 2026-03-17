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
    CComPtr<IDxcBlob> Output;
    ASSERT_EQ(BShaderCompiler::CompileShaderSource(ComputeShaderSource, "CSMain", "cs_6_0", &Output), SHADER_COMPILER_RESULT_OK);
}
