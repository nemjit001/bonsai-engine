#include <array>
#include <gtest/gtest.h>
#include "RenderBackend/RenderResourcePool.hpp"
#include "RenderBackend/ShaderCompiler.hpp"

struct MockResource
{
    int Value;
};

TEST(RendererTests, TestResourcePool)
{
    TRenderResourcePool<MockResource> ResourcePool;
    TRenderHandle<MockResource> Handle = ResourcePool.Allocate(MockResource{ 0x42 });

    MockResource Resource{};
    ASSERT_TRUE(ResourcePool.GetResource(Handle, Resource));
    ASSERT_EQ(Resource.Value, 0x42);

    ResourcePool.Free(Handle);
    ASSERT_FALSE(ResourcePool.IsValidResource(Handle));
}

static constexpr char const* ComputeShaderSource = R"(
[shader("compute")]
[numthreads(1, 1, 1)]
void CSMain()
{
    // Nothing to see here...
}
)";

static constexpr std::array TestShaderCompilerArgs = {
    DXC_ARG_GENERATE_SPIRV, // Spir-V bytecode gen works on all target platforms
};

TEST(RendererTests, TestCompileShaderSource)
{
    BShaderCompilerArgs CompilerArgs{};
    CompilerArgs.Arguments = TestShaderCompilerArgs.data();
    CompilerArgs.ArgumentCount = TestShaderCompilerArgs.size();
    CompilerArgs.IncludePaths = nullptr;
    CompilerArgs.IncludePathCount = 0;
    CompilerArgs.Defines = nullptr;
    CompilerArgs.DefineCount = 0;

    CComPtr<IDxcBlob> Output;
    ASSERT_EQ(BShaderCompiler::CompileShaderSource(ComputeShaderSource, "CSMain", "cs_6_0", CompilerArgs, &Output), SHADER_COMPILER_RESULT_OK);
}
