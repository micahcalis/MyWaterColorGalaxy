#pragma once

#include "System/Serialization/SerializableGalaxy.hpp"
#include "System/Delegates/Delegate.hpp"
#include <string>

namespace Beer::System
{
    struct MapHandler
    {
    private:
        std::string mapName;
        Function<SerializableGalaxy, const std::string&> loadFunc = nullptr;
        Function<void, const std::string&, const SerializableGalaxy&> saveFunc = nullptr;
        Function<bool, const std::string&> isMapSaved = nullptr;

    public:
        MapHandler(const std::string& mapName,
            Function<SerializableGalaxy, const std::string&> loadFunc,
            Function<void, const std::string&, const SerializableGalaxy&> saveFunc,
            Function<bool, const std::string&> isMapSaved)
            : mapName(mapName)
            , loadFunc(loadFunc)
            , saveFunc(saveFunc)
            , isMapSaved(isMapSaved)
        {
        }

        SerializableGalaxy Load()
        {
            return loadFunc(mapName);
        }

        void Save(const SerializableGalaxy& map)
        {
            saveFunc(mapName, map);
        }

        bool IsSaved()
        {
            return isMapSaved(mapName);
        }
    };
} // namespace Beer::System
