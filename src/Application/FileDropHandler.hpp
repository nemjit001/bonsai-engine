#pragma once
#ifndef BONSAI_FILE_DROP_HANDLER_HPP
#define BONSAI_FILE_DROP_HANDLER_HPP

#include <Core/Filesystem.hpp>
#include "Assets/AssetManager.hpp"
#include "Scene/SceneManager.hpp"

namespace Bonsai
{
    /// @brief The FileDropHandler is an application helper that handles file drop events.
    class FileDropHandler
    {
    private:
        FileDropHandler() = default;

    public:
        /// @brief Handle having a file dropped into the application. This will attempt to load the file as an asset.
        /// @param path Path to the dropped file.
        /// @param assetManager Asset manager to use for asset loading.
        /// @param sceneManager Scene manager to use for scene queries.
        static void HandleDroppedFile(Filesystem::Path const& path, AssetManager& assetManager, SceneManager& sceneManager);
    };
} // namespace Bonsai

#endif //BONSAI_FILE_DROP_HANDLER_HPP
