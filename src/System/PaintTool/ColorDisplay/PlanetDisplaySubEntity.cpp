#include "System/PaintTool/ColorDisplay/PlanetDisplaySubEntity.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"

namespace Beer::System
{
    static const float CONTAINER_SCALE = 0.475f;
    static const glm::vec2 CONTAINER_OFFSET = glm::vec2(0.025f, 0.325f);
    static const float DISPLAY_SCALE = 0.38f;

    static const uint32_t SPRITE_COUNT = 5;
    static const std::array<std::string, SPRITE_COUNT> SPRITE_PATHS = {
        "UI/ToolBar/Tex_PlanetButton",
        "UI/ToolBar/Tex_AsteroidsButton",
        "UI/ToolBar/Tex_SpacegooButton",
        "UI/ToolBar/Tex_BlackholeButton",
        "UI/ToolBar/Tex_StardustButton"};

    static const std::array<std::string, SPRITE_COUNT> MASK_PATHS = {
        "UI/ToolBar/Tex_PlanetMask",
        "UI/ToolBar/Tex_AsteroidsMask",
        "UI/ToolBar/Tex_SpacegooMask",
        "UI/ToolBar/Tex_BlackholeMask",
        "UI/ToolBar/Tex_StardustMask"};

    PlanetDisplaySubEntity::PlanetDisplaySubEntity(UITransform* parent)
    {
        UITransform containerTransform{};
        containerTransform.Anchor = AnchorMode::Center;
        containerTransform.Pivot = AnchorMode::Center;
        containerTransform.Scale = glm::vec2(CONTAINER_SCALE);
        containerTransform.Position = CONTAINER_OFFSET;

        container = std::make_unique<UISubEntity>(containerTransform);
        parent->BindChild(container->GetTransform());

        containerTexture = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_PlanetDisplayFrame");
        containerMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        containerMaterial->SetTexture("_SpriteTex", containerTexture.get());
        containerMaterial->SetColor("_TintColor", glm::vec4(1));
        containerMaterial->SetVector("_Scale", glm::vec4(1));

        UITransform displayTransform{};
        displayTransform.Anchor = AnchorMode::Center;
        displayTransform.Pivot = AnchorMode::Center;
        displayTransform.Scale = glm::vec2(DISPLAY_SCALE);

        displayEntity = std::make_unique<UISubEntity>(displayTransform);
        container->GetTransform()->BindChild(displayEntity->GetTransform());

        InitializeSpriteMaps();

        displayMaterial = std::make_shared<Rendering::Material>("UI/GalaxyComponentSprite");
        displayMaterial->SetInt("_OverrideMapClip", 1);
        displayMaterial->SetTexture("_SpriteTex", spriteMap.at(GalaxyBrushType::Planet).get());
        displayMaterial->SetTexture("_ColorMask", maskMap.at(GalaxyBrushType::Planet).get());
        displayMaterial->SetColor("_PrimaryColor", glm::vec4(1, 0, 0, 1));
    }

    void PlanetDisplaySubEntity::InitializeSpriteMaps()
    {
        for (uint32_t i = 0; i < SPRITE_COUNT; i++)
        {
            GalaxyBrushType type = static_cast<GalaxyBrushType>(i);
            spriteMap[type] = std::make_shared<Rendering::Texture2D>(SPRITE_PATHS[i]);
            maskMap[type] = std::make_shared<Rendering::Texture2D>(MASK_PATHS[i]);
        }
    }

} // namespace Beer::System
