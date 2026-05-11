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
        Function<SerializablePaintSession, const std::string&> load = [this](const std::string& name) -> SerializablePaintSession {
            return factory->LoadMap(name);
        };

        Function<void, const std::string&, const SerializablePaintSession&> save =
            [this](const std::string& name, const SerializablePaintSession& map) -> void {
            factory->SaveMap(name, map);
        };

        Function<bool, const std::string&> hasMap = [this](const std::string& name) -> bool {
            return factory->Has(name);
        };

        return MapHandler(name, load, save, hasMap);
    }

} // namespace Beer::System
