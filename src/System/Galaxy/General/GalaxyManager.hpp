#pragma once

#include "System/Serialization/SerializableGalaxy.hpp"
#include "System/Galaxy/General/GalaxyContainer.hpp"
#include "System/Components/Registry/IEntityManager.hpp"

namespace Beer::System
{
    class GalaxyManager : public IEntityManager
    {
    private:
        GalaxyContainer* container = nullptr;

    public:
        GalaxyManager(GalaxyContainer* container)
            : container(container)
        {
        }

        void Update() override
        {
            UpdateOrbits();
        }

        void Load(const SerializableGalaxy& serializedGalaxy);
        void Clear();

    private:
        void UpdateOrbits();
    };
} // namespace Beer::System
