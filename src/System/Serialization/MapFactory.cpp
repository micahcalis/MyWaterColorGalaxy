#include "System/Serialization/MapFactory.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "SerializableGalaxy.hpp"
#include <format>
#include <fstream>
#include <filesystem>
#include <print>
#include <stdexcept>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace Beer::System
{
    const std::string MAP_DIRECTORY = "assets/maps/";

    MapFactory::MapFactory()
    {
        CollectSavedGalaxyPaths();
    }

    SerializableGalaxyMap MapFactory::LoadMap(const std::string& name)
    {
        std::filesystem::path filepath = Core::AssetUtilities::GetMapAssetPath(name);
        std::ifstream file(filepath);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open map file: " + filepath.generic_string());
        }

        json j;
        file >> j;

        return j.get<SerializableGalaxyMap>();
    }

    void MapFactory::SaveMap(const std::string& name, const SerializableGalaxyMap& galaxy)
    {
        std::filesystem::path filepath = Core::AssetUtilities::GetMapAssetPath(name);
        std::ofstream file(filepath);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to create map file: " + filepath.generic_string());
        }

        json j = galaxy;
        file << j.dump(4);

        if (!Has(name))
        {
            mapNames.push_back(name);
        }

        std::println("Saved Map '{}', at: {}", name, filepath.generic_string());
    }

    void MapFactory::CollectSavedGalaxyPaths()
    {
        mapNames.clear();

        std::filesystem::path directory = Core::AssetUtilities::GetBasePath(MAP_DIRECTORY);

        if (!fs::exists(directory))
            return;

        for (const auto& entry : fs::directory_iterator(directory))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                mapNames.push_back(entry.path().stem().string());
                std::println("Found Map '{}', at: {}", entry.path().stem().string(), entry.path().generic_string());
            }
        }
    }

    std::string MapFactory::GetMapNameByIndex(uint32_t index) const
    {
        if (index >= mapNames.size())
        {
            return "";
        }

        return mapNames.at(index);
    }

    bool MapFactory::Has(const std::string& name) const
    {
        return std::ranges::contains(mapNames, name);
    }
} // namespace Beer::System
