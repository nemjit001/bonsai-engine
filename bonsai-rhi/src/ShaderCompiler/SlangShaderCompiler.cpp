#include "SlangShaderCompiler.hpp"

namespace RHI
{
    SlangShaderCompiler::SlangShaderCompiler(Slang::ComPtr<slang::IGlobalSession>&& globalSession)
        :
        _globalSession(globalSession)
    {
        //
    }

    SlangShaderCompiler* SlangShaderCompiler::TryCreate()
    {
        Slang::ComPtr<slang::IGlobalSession> globalSession = nullptr;
        if (!SLANG_SUCCEEDED(slang::createGlobalSession(globalSession.writeRef()))) {
            return nullptr;
        }

        return new SlangShaderCompiler(std::move(globalSession));
    }
} // namespace RHI

