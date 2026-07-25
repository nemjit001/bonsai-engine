#include "RHIShaderCompiler.hpp"

#include "ShaderCompiler/ShaderBlob.hpp"
#include "ShaderCompiler/SlangShaderCompiler.hpp"

namespace RHI
{
    IRHIShaderCompiler* BONSAI_RHI_APICALL CreateShaderCompiler()
    {
        return SlangShaderCompiler::TryCreate();
    }

    void BONSAI_RHI_APICALL DestroyShaderCompiler(IRHIShaderCompiler* shaderCompiler)
    {
        delete shaderCompiler;
    }

    IShaderBlob* BONSAI_RHI_APICALL CreateShaderBlob(void const* data, usize size)
    {
        return new ShaderBlob(data, size);
    }

    void BONSAI_RHI_APICALL DestroyShaderBlob(IShaderBlob* blob)
    {
        delete blob;
    }
} // namespace RHI

