#pragma once
#ifndef BONSAI_RENDER_OBJECT_CACHE_HPP
#define BONSAI_RENDER_OBJECT_CACHE_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <entt/entt.hpp>

namespace Bonsai
{
    /// @brief The RenderObjectId is an internal ID for referencing render objects.
    using RenderObjectId = uint32_t;

    /// @brief The RenderObjectCache maps ECS entities to stable render object IDs.
    class RenderObjectCache
    {
    public:
        /// @brief Insert a new entity in the cache.
        /// @param entity Entity to insert.
        /// @return The render object ID associated with the entity.
        RenderObjectId Insert(entt::entity const& entity);

        /// @brief Mark an entity as used for this cache update.
        /// @param entity Entity to mark as used.
        void MarkUsed(entt::entity const& entity);

        /// @brief Update the cache entries, freeing unused object IDs for reuse.
        void Update();

        /// @brief Check if an ECS entity has a render object ID.
        /// @param entity Entity to use for cache query.
        /// @return `true` if the cache contains a render object ID for the entity.
        bool HasRenderObjectId(entt::entity const& entity) const;

        /// @brief Get the render object ID for an ECS entity.
        /// @param entity Entity to use for cache query.
        /// @return The associated render object ID.
        RenderObjectId GetRenderObjectId(entt::entity const& entity) const;

        /// @brief Get the number of render objects in the cache. This is a worst-case bound if all cache entries are used.
        /// @return The number of render objects in the cache.
        inline size_t GetRenderObjectCount() const { return _maxRenderObjectId; }

    private:
        struct Entry
        {
            bool used;
            RenderObjectId objectId;
        };

        /// @brief Get the next free render object ID.
        /// @return The next free object ID.
        RenderObjectId GetNextFreeRenderObjectId();

    private:
        RenderObjectId _maxRenderObjectId = 0;
        std::vector<RenderObjectId> _renderObjectFreeList;
        std::vector<entt::entity> _deletionQueue;
        std::unordered_map<entt::entity, Entry> _renderObjectCache;
    };
} // namespace Bonsai

#endif //BONSAI_RENDER_OBJECT_CACHE_HPP
