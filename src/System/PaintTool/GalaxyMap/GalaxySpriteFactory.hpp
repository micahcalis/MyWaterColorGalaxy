#pragma once

#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "Vendor/magic_enum/magic_enum.hpp"
#include <array>
#include <format>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace Beer::System
{
    static const uint32_t BRUSH_TYPES = 5;

    static const std::array<std::string, BRUSH_TYPES> SPRITE_PATHS = {
        "UI/ToolBar/Tex_PlanetButton",
        "UI/ToolBar/Tex_AsteroidsButton",
        "UI/ToolBar/Tex_SpacegooButton",
        "UI/ToolBar/Tex_BlackholeButton",
        "UI/ToolBar/Tex_StardustButton"};

    static const std::array<std::string, BRUSH_TYPES> SHADER_PATHS = {
        "UI/GalaxyComponentSprite",
        "UI/GalaxyComponentSprite",
        "UI/GalaxyComponentSprite",
        "UI/GalaxyComponentSprite",
        "UI/GalaxyComponentSprite"};

    static const std::array<std::string, 4> COLOR_PROPERTIES = {
        "_PrimaryColor",
        "_SecondaryColor",
        "_TertiaryColor",
        "_QuaternaryColor"};

    class GalaxySpriteFactory
    {
    private:
        std::unordered_map<GalaxyBrushType, std::shared_ptr<Rendering::Texture2D>> spriteMap;
        std::unordered_map<GalaxyBrushType, std::shared_ptr<Rendering::Shader>> shaderMap;
        Function<glm::vec4, ColorBarLevel> getColor = nullptr;

    public:
        GalaxySpriteFactory(Function<glm::vec4, ColorBarLevel> getColor)
            : getColor(getColor)
        {
            InitializeSpriteMap();
            InitializeShaderMap();
        }

        std::shared_ptr<Rendering::Material> GetMaterial(const GalaxyBrushType type) const
        {
            auto it1 = spriteMap.find(type);
            auto it2 = shaderMap.find(type);

            if (it1 == spriteMap.end() || it2 == shaderMap.end())
            {
                throw std::runtime_error(std::format("Brush Type Uninitialized in Galaxy Map Factory: {}", magic_enum::enum_name(type)));
            }

            std::shared_ptr<Rendering::Material> newMaterial = std::make_shared<Rendering::Material>(shaderMap.at(type));
            newMaterial->SetTexture("_SpriteTex", spriteMap.at(type).get());

            for (uint32_t i = 0; i < 4; i++)
            {
                newMaterial->SetColor(COLOR_PROPERTIES[i], getColor(static_cast<ColorBarLevel>(i)));
            }

            return newMaterial;
        }

        std::vector<glm::vec4> GetColors() const
        {
            std::vector<glm::vec4> colors;

            for (uint32_t i = 0; i < 4; i++)
            {
                colors.push_back(getColor(static_cast<ColorBarLevel>(i)));
            }

            return colors;
        }

    private:
        void InitializeSpriteMap()
        {
            for (uint32_t i = 0; i < BRUSH_TYPES; i++)
            {
                GalaxyBrushType type = static_cast<GalaxyBrushType>(i);
                spriteMap[type] = std::make_shared<Rendering::Texture2D>(SPRITE_PATHS[i]);
            }
        }

        void InitializeShaderMap()
        {
            for (uint32_t i = 0; i < BRUSH_TYPES; i++)
            {
                GalaxyBrushType type = static_cast<GalaxyBrushType>(i);
                shaderMap[type] = Rendering::Shader::Get(SHADER_PATHS[i]);
            }
        }
    };
} // namespace Beer::System
