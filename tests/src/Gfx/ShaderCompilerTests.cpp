#include <gtest/gtest.h>
#include "Gfx/ShaderCompiler.hpp"

class ShaderCompilerTests : public testing::Test
{
public:
    static void SetUpTestSuite()
    {
        // Create shader compiler
        _shaderCompiler = Bonsai::ShaderCompiler::TryCreate();
        ASSERT_NE(_shaderCompiler, nullptr);
    }

    static void TearDownTestSuite()
    {
        // Delete the shader compiler
    Bonsai::ShaderCompiler::Destroy(_shaderCompiler);
    }

protected:
    inline static Bonsai::ShaderCompiler* _shaderCompiler = nullptr;
};

TEST_F(ShaderCompilerTests, TestCompileFromFile)
{
    Slang::ComPtr<slang::IBlob> shader = _shaderCompiler->CompileShaderFromFile(Bonsai::Filesystem::GetResourcePath("resources/shaders/test_shader.slang"));
    ASSERT_NE(shader, nullptr);
}

TEST_F(ShaderCompilerTests, TestCompileFromInvalidFilePath)
{
    Slang::ComPtr<slang::IBlob> shader = _shaderCompiler->CompileShaderFromFile(Bonsai::Filesystem::GetResourcePath("resources/shaders/DOES_NOT_EXIST"));
    ASSERT_EQ(shader, nullptr);
}
