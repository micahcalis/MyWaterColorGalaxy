#pragma once

#include "GalaxyContainer.hpp"
#include "GalaxyManager.hpp"
#include "GalaxyRenderComponent.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Components/Registry/IEntity.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <memory>
#include <unordered_map>

namespace Beer::System
{
    class GalaxyEntity : public GameEntity
    {
    private:
        std::unique_ptr<GalaxyContainer> container = nullptr;

    public:
        GalaxyEntity()
            : GameEntity(Transform(), nullptr)
        {
            container = std::make_unique<GalaxyContainer>();
            renderComponent = std::make_unique<GalaxyRenderComponent>(container.get());
        }

        void Update() override
        {
            manager->Update();
        }

        void LoadFromSerialized(const SerializableGalaxy& serializedGalaxy)
        {
            GetGalaxyManager()->Load(serializedGalaxy);
        }

        void ClearGalaxy()
        {
            GetGalaxyManager()->Clear();
        }

        GalaxyManager* GetGalaxyManager() const { return static_cast<GalaxyManager*>(manager.get()); }

    private:
        void InitializeManager() override
        {
            manager = std::make_unique<GalaxyManager>(container.get());
        }
    };
} // namespace Beer::System
