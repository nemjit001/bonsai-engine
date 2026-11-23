#include "RenderObjectCache.hpp"

namespace Bonsai
{
    RenderObjectId RenderObjectCache::Insert(entt::entity const& entity)
    {
        RenderObjectId const objectId = GetNextFreeRenderObjectId();
        _renderObjectCache.insert({ entity, Entry{ true, objectId } });
        return objectId;
    }

    void RenderObjectCache::MarkUsed(entt::entity const& entity)
    {
        auto const iter = _renderObjectCache.find(entity);
        if (iter == _renderObjectCache.end()) {
            return;
        }

        Entry& entry = iter->second;
        entry.used = true;
    }

    void RenderObjectCache::Update()
    {
        _deletionQueue.clear();
        for (auto& [ entity, entry ] : _renderObjectCache)
        {
            // Mark unused entries as free
            if (!entry.used) {
                _renderObjectFreeList.push_back(entry.objectId);
                _deletionQueue.push_back(entity);
            }

            // Clear used flag
            entry.used = false;
        }

        for (auto const& entity : _deletionQueue) {
            _renderObjectCache.erase(entity);
        }
    }

    bool RenderObjectCache::HasRenderObjectId(entt::entity const& entity) const
    {
        return _renderObjectCache.find(entity) != _renderObjectCache.end();
    }
    
    RenderObjectId RenderObjectCache::GetRenderObjectId(entt::entity const& entity) const
    {
        Entry const& entry = _renderObjectCache.at(entity);
        return entry.objectId;
    }

    RenderObjectId RenderObjectCache::GetNextFreeRenderObjectId()
    {
        // Attempt to reuse object IDs
        if (!_renderObjectFreeList.empty())
        {
            RenderObjectId const objectId = _renderObjectFreeList.back();
            _renderObjectFreeList.pop_back();
            return objectId;
        }

        // Use next object ID
        RenderObjectId const objectId = _maxRenderObjectId;
        _maxRenderObjectId += 1;
        return objectId;
    }
} // namespace Bonsai
