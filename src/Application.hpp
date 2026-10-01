#pragma once
#ifndef BONSAI_APPLICATION_HPP
#define BONSAI_APPLICATION_HPP

namespace Bonsai
{
    /// @brief Enumeration of return codes for the Application class.
    enum AppResult : int
    {
        AppResult_Ok        = 0,
        AppResult_BadInit   = 1,
    };

    /// @brief The Application class manages the application lifecyle.
    class Application
    {
    private:
        /// @brief Initialize the application.
        /// @return `true` on success.
        bool Init();

        /// @brief Shut down the application.
        void Shutdown();

    public:
        /// @brief Run the application.
        /// @return 
        AppResult Run();
    };
} // namespace Bonsai


#endif //BONSAI_APPLICATION_HPP
