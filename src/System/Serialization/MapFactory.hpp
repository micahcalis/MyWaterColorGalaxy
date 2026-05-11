#pragma once

#include "System/Serialization/SerializableGalaxy.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>

namespace Beer::System
{
    class MapFactory
    {
    private:
        std::vector<std::string> mapNames;

    public:
        MapFactory();
        SerializableGalaxy LoadMap(const std::string& name);
        void SaveMap(const std::string& name, const SerializableGalaxy& galaxy);
        std::string GetMapNameByIndex(uint32_t index) const;
        bool Has(const std::string& name) const;

    private:
        void CollectSavedGalaxyPaths();
    };
} // namespace Beer::System
