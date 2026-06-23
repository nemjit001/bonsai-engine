#pragma once
#ifndef BONSAI_ENGINE_HPP
#define BONSAI_ENGINE_HPP

#include <BonsaiBase.hpp>

class World;

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
    /// @param argc Number of passed CLI arguments.
    /// @param argv Array of CLI arguments passed to the application.
    /// @return A boolean indicating successful initialization.
    bool Init(int argc, char* argv[]);

    /// @brief Tick the engine state.
    void Tick();

public:
    /// @brief Run the engine application.
    /// @param argc Number of passed CLI arguments.
    /// @param argv Array of CLI arguments passed to the application.
    /// @return A BonsaiEngineResult return code.
    int32 Run(int argc, char* argv[]);

private:
    bool _isRunning = false;
    std::unique_ptr<World> _world = nullptr;
};

#endif //BONSAI_ENGINE_HPP
