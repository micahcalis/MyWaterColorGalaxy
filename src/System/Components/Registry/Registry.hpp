#pragma once
#include "System/Components/Registry/GameEntity.hpp"
#include <unordered_map>
#include <memory>

namespace Beer::System
{
    class Registry
    {
    private:
        std::unordered_map<uint32_t, std::unique_ptr<GameEntity>> registeredEntities;
        uint32_t instanceCounter = 0;

    public:
        template<typename T, typename... Args>
        T* CreateEntity(Args&&... args)
        {
            static_assert(std::is_base_of<GameEntity, T>::value, "type must inherit from GameEntity");

            instanceCounter++;
            uint32_t id = instanceCounter;
            T* entity = new T(std::forward<Args>(args)...);
            entity->SetId(this, id);
            static_cast<GameEntity*>(entity)->InitializeManager();

            registeredEntities.emplace(id, std::unique_ptr<GameEntity>(entity));
            return entity;
        }

        template<typename T>
        T* GetEntity(uint32_t id)
        {
            static_assert(std::is_base_of<GameEntity, T>::value, "type must inherit from GameEntity");

            auto it = registeredEntities.find(id);
            if (it != registeredEntities.end())
                return static_cast<T*>(it->second.get());

            return nullptr;
        }

        void DestroyEntity(uint32_t id)
        {
            registeredEntities.erase(id);
        }

        void Clear()
        {
            registeredEntities.clear();
        }
    };
} // namespace Beer::System
