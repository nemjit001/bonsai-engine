#include "Application.hpp"

namespace Bonsai
{
    bool Application::Init()
    {
        return true;
    }

    void Application::Shutdown()
    {
        //
    }

    AppResult Application::Run()
    {
        if (!Init()) {
            return AppResult_BadInit;
        }

        Shutdown();
        return AppResult_Ok;
    }
} // namespace Bonsai
