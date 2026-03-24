#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <memory>

class IRenderBackend;
class BRenderer;

/// @brief Error codes for the engine class 
enum EngineResult : uint32_t
{
    ENGINE_RESULT_OK                = 0x00000000,
    ENGINE_RESULT_PLATFORM_BAD_INIT = 0x00010000,
    ENGINE_RESULT_RENDERER_BAD_INIT = 0x00020000,
};

/// @brief The Engine class wraps the startup and shutdown of engine systems.
class BEngine
{
public:
    BEngine();
    ~BEngine();

    BEngine(BEngine const&) = delete;
    BEngine& operator=(BEngine const&) = delete;

    BEngine(BEngine&&) = delete;
    BEngine& operator=(BEngine&&) = delete;

    /// @brief Run the engine main loop.
    /// @return 
    EngineResult Run();

private:
    /// @brief Initialize engine systems.
    /// @return 
    EngineResult Init();

    /// @brief Shut down engine systems.
    void Shutdown();

private:
    std::unique_ptr<IRenderBackend> RenderBackend;
    std::unique_ptr<BRenderer> Renderer;
};

#endif //ENGINE_HPP
