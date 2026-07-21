#include <gtest/gtest.h>
#include <BonsaiRHI.hpp>
#include <RHIShaderCompiler.hpp>

/// @brief RHI shader compiler test fixture.
class RHIShaderCompilerTests : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        _shaderCompiler = RHI::CreateShaderCompiler();
        ASSERT_NE(_shaderCompiler, nullptr);

        // Set target language to SPIR-V, which is always supported.
        _shaderCompiler->SetTargetLanguage(RHI::ShaderLang_SPIRV);
    }

    static void TearDownTestCase()
    {
        RHI::DestroyShaderCompiler(_shaderCompiler);
        _shaderCompiler = nullptr;
    }

protected:
    inline static RHI::IRHIShaderCompiler* _shaderCompiler = nullptr;
};

TEST_F(RHIShaderCompilerTests, TestCompileShaderSourceString)
{
    constexpr char const* SHADER_SOURCE = R"(
    [shader("compute")]
    [numthreads(1,1,1)]
    void CSMain(uint3 threadId : SV_DispatchThreadID) {
        //
    }
    )";

    RHI::ShaderBlob shaderBlob{};
    EXPECT_TRUE(_shaderCompiler->CompileShaderSource("CSMain", SHADER_SOURCE, &shaderBlob));
    EXPECT_NE(shaderBlob.code, nullptr);
    EXPECT_GT(shaderBlob.size, 0);
}

TEST_F(RHIShaderCompilerTests, TestCompileShaderFile)
{
    fs::Path const shaderPath = fs::GetProgramDirectory()/"test_assets/shaders/minimal_shader.slang";

    RHI::ShaderBlob shaderBlob{};
    EXPECT_TRUE(_shaderCompiler->CompileShaderFile("CSMain", shaderPath, &shaderBlob));
    EXPECT_NE(shaderBlob.code, nullptr);
    EXPECT_GT(shaderBlob.size, 0);
}

TEST_F(RHIShaderCompilerTests, TestCompileShaderFileResolveIncludes)
{
    fs::Path const shaderPath = fs::GetProgramDirectory()/"test_assets/shaders/shader_with_imports.slang";

    RHI::ShaderBlob shaderBlob{};
    EXPECT_TRUE(_shaderCompiler->CompileShaderFile("CSMain", shaderPath, &shaderBlob));
    EXPECT_NE(shaderBlob.code, nullptr);
    EXPECT_GT(shaderBlob.size, 0);
}
