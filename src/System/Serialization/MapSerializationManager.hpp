#pragma once

#include "System/Serialization/MapHandler.hpp"
#include "System/Serialization/MapFactory.hpp"
#include <memory>

namespace Beer::System
{
    class MapSerializationManager
    {
    private:
        std::unique_ptr<MapFactory> factory = nullptr;

    public:
        MapSerializationManager();
        MapHandler GetMapHandler(const std::string& name) const;
    };
} // namespace Beer::System
