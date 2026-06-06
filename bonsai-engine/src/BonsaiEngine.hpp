#pragma once
#ifndef BONSAI_ENGINE_HPP
#define BONSAI_ENGINE_HPP

#include <BonsaiBase.hpp>

/// @brief Enumeration of engine return codes.
enum BonsaiEngineResult : int32
{
    BonsaiEngineResult_Ok       = 0,
    BonsaiEngineResult_BadInit  = 1,
};

/// @brief The BonsaiEngine class acts as the application runtime, it manages startup, lifetime, and shutdown of engine systems.
class BonsaiEngine
{
public:
    BonsaiEngine();
    ~BonsaiEngine();

    BonsaiEngine(BonsaiEngine const&) = delete;
    BonsaiEngine& operator=(BonsaiEngine const&) = delete;

private:
    /// @brief Initialize the engine subsystems.
    /// @return 
    bool Init();

    /// @brief Tick the engine state.
    void Tick();

public:
    /// @brief Run the engine application.
    /// @return A BonsaiEngineResult return code.
    int32 Run();

private:
    bool _isRunning = false;
};

#endif //BONSAI_ENGINE_HPP
