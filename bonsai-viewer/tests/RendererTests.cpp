#include <gtest/gtest.h>
#include "Renderer/RenderResourcePool.hpp"
#include "Renderer/ShaderCompiler.hpp"

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

TEST(RendererTests, TestCompileComputeShader)
{
    constexpr char const* ComputeShader = R"(
[shader("compute")]
[numthreads(1, 1, 1)]
void CSMain()
{
}
)";

    BShaderSource ShaderSource{};
    ShaderSource.Stage = ShaderStage_Compute;
    ShaderSource.Entrypoint = "CSMain";
    ShaderSource.Source = ComputeShader;
    ShaderSource.SourceLength = std::strlen(ComputeShader);

    CComPtr<IDxcBlob> CompiledShader{};
    ASSERT_EQ(BShaderCompiler::Compile(ShaderLang_HLSL, CompilerOutput_SPIRV, TARGET_PROFILE_COMPUTE_SHADER("6_0"), ShaderSource, &CompiledShader), SHADER_COMPILE_RESULT_OK);
}
