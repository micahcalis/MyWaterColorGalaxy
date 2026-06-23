#pragma once

#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "glm/fwd.hpp"
#include <stdexcept>
#include <unordered_map>
namespace Beer::System
{
    class ColorDisplayHandler
    {
    private:
        std::unordered_map<ColorBarLevel, std::shared_ptr<Rendering::Material>> colorMap;

    public:
        void AddLevel(ColorBarLevel level, std::shared_ptr<Rendering::Material> material)
        {
            if (colorMap.contains(level))
            {
                throw std::runtime_error("Color Level already exists in ColorDisplayHandler!");
            }

            colorMap[level] = material;
        }

        void SetColor(glm::vec4 color, ColorBarLevel level)
        {
            auto it = colorMap.find(level);

            if (it == colorMap.end())
            {
                throw std::runtime_error("Color Display doesn't have required Level in map!");
            } else
            {
                it->second->SetColor("_TintColor", color);
            }
        }
    };
} // namespace Beer::System
