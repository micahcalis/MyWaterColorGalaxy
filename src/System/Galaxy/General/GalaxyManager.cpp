#include "System/Galaxy/General/GalaxyManager.hpp"
#include "GalaxyManager.hpp"
#include "System/Galaxy/General/GalaxyContainer.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"

namespace Beer::System
{
    void GalaxyManager::Load(const SerializableGalaxy& serializedGalaxy)
    {
        Clear();
        container->CreateBuffers(serializedGalaxy);
    }

    void GalaxyManager::Clear()
    {
        container->Clear();
    }

    void GalaxyManager::UpdateOrbits()
    {
        container->Update();
    }

} // namespace Beer::System
