#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>

enum RendererResult : int
{
    RENDERER_RESULT_OK                      = 0x00,
    RENDERER_RESULT_SYMBOL_LOADING_BAD_INIT = 0x10,
    RENDERER_RESULT_WINDOW_BAD_INIT         = 0x20,
    RENDERER_RESULT_GFX_API_BAD_INIT        = 0x30,
    RENDERER_RESULT_UNSUPPORTED_GFX_DEVICE  = 0x40,
};

class BRenderer
{
public:
    BRenderer();
    ~BRenderer();

    BRenderer(BRenderer const&) = delete;
    BRenderer& operator=(BRenderer const&) = delete;

    BRenderer(BRenderer&&) = delete;
    BRenderer& operator=(BRenderer&&) = delete;

    RendererResult Init();

    void Shutdown();

    void Frame();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif //RENDERER_HPP
