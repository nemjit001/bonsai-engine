#include "RHIShaderCompiler.hpp"

#include "ShaderCompiler/SlangShaderCompiler.hpp"

namespace RHI
{
    IRHIShaderCompiler* BONSAI_RHI_APICALL CreateShaderCompiler()
    {
        return new SlangShaderCompiler();
    }

    void BONSAI_RHI_APICALL DestroyShaderCompiler(IRHIShaderCompiler* shaderCompiler)
    {
        delete shaderCompiler;
    }
} // namespace RHI

