#pragma once

#include <entt/entt.hpp>

namespace Beer::System
{
    using Entity = entt::entity;
    using EntityHandle = entt::handle;

    template<typename... ComponentTypes>
    using Collection = entt::view<entt::get_t<ComponentTypes...>>;
} // namespace Beer::System
