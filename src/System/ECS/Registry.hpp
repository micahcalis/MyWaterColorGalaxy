#pragma once

#include "entt/entity/fwd.hpp"
#include <entt/entt.hpp>
#include "System/ECS/ECS.hpp"

namespace Beer::System
{
    class Registry
    {
    private:
        entt::registry registry;

    public:
        entt::registry* GetEnttHandle() { return &registry; }

        Entity Create()
        {
            return registry.create();
        }

        void Destroy(Entity entity)
        {
            registry.destroy(entity);
        }

        EntityHandle GetHandle(Entity entity)
        {
            return EntityHandle{registry, entity};
        }

        template<typename T, typename... Args>
        decltype(auto) AddComponent(Entity entity, Args&&... args)
        {
            return registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename T>
        void RemoveComponent(Entity entity)
        {
            registry.erase<T>(entity);
        }

        template<typename T>
        T& GetComponent(Entity entity)
        {
            return registry.get<T>(entity);
        }

        template<typename T>
        T* TryGetComponent(Entity entity)
        {
            return registry.try_get<T>(entity);
        }

        template<typename... ComponentTypes>
        bool HasComponent(Entity entity) const
        {
            return registry.all_of<ComponentTypes...>(entity);
        }

        template<typename... ComponentTypes>
        auto GetAll()
        {
            return registry.view<ComponentTypes...>();
        }
    };
} // namespace Beer::System
