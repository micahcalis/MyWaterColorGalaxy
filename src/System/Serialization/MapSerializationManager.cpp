#include "System/Serialization/MapSerializationManager.hpp"
#include "System/Serialization/MapFactory.hpp"
#include "System/Serialization/MapHandler.hpp"

namespace Beer::System
{
    MapSerializationManager::MapSerializationManager()
    {
        factory = std::make_unique<MapFactory>();
    }

    MapHandler MapSerializationManager::GetMapHandler(const std::string& name) const
    {
        Function<SerializableGalaxyMap, const std::string&> load = [this](const std::string& name) -> SerializableGalaxyMap {
            return factory->LoadMap(name);
        };

        Function<void, const std::string&, const SerializableGalaxyMap&> save =
            [this](const std::string& name, const SerializableGalaxyMap& map) -> void {
            factory->SaveMap(name, map);
        };

        Function<bool, const std::string&> hasMap = [this](const std::string& name) -> bool {
            return factory->Has(name);
        };

        return MapHandler(name, load, save, hasMap);
    }

} // namespace Beer::System
