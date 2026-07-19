#include <gtest/gtest.h>
#include <RHIShaderCompiler.hpp>

/// @brief RHI shader compiler test fixture.
class RHIShaderCompilerTests : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        _shaderCompiler = RHI::CreateShaderCompiler();
        ASSERT_NE(_shaderCompiler, nullptr);
    }

    static void TearDownTestCase()
    {
        RHI::DestroyShaderCompiler(_shaderCompiler);
        _shaderCompiler = nullptr;
    }

private:
    inline static RHI::IRHIShaderCompiler* _shaderCompiler = nullptr;
};

TEST_F(RHIShaderCompilerTests, TestCompileShaderSourceString)
{
    //
}

TEST_F(RHIShaderCompilerTests, TestCompileShaderFile)
{
    //
}

TEST_F(RHIShaderCompilerTests, TestCompileShaderFileResolveIncludes)
{
    //
}
