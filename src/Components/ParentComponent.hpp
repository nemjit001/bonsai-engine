#pragma once
#ifndef BONSAI_PARENT_COMPONENT_HPP
#define BONSAI_PARENT_COMPONENT_HPP

#include <entt/entt.hpp>

namespace Bonsai
{
    /// @brief The ParentComponent parents one entity to another.
    /// This component should be attached to the child entity.
    struct ParentComponent
    {
        entt::entity parent = entt::null;
    };
} // namespace Bonsai

#endif //BONSAI_PARENT_COMPONENT_HPP
