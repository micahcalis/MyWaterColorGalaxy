#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include <memory>

namespace Beer::System
{
    struct GalaxyComponentData
    {
        GalaxyBrushType Brush = GalaxyBrushType::Planet;
        std::vector<glm::vec4> Colors;
        float Scale = 0;
        glm::vec2 Position = glm::vec2(0);
        glm::vec2 Tilt = glm::vec2(0);
    };

    class GalaxyComponent
    {
    public:
        std::unique_ptr<UISubEntity> Entity = nullptr;
        std::shared_ptr<Rendering::Material> Material = nullptr;
        GalaxyComponentData Data{};

    private:
        uint32_t id = 0;

    public:
        GalaxyComponent(std::unique_ptr<UISubEntity> entity,
            std::shared_ptr<Rendering::Material> material,
            GalaxyComponentData data)
            : Entity(std::move(entity)), Material(material), Data(data)
        {
        }

        void SetId(uint32_t id)
        {
            this->id = id;
        }
    };
} // namespace Beer::System
