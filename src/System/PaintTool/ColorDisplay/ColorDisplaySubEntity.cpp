#include "System/PaintTool/ColorDisplay/ColorDisplaySubEntity.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <utility>

namespace Beer::System
{
    static const int NUM_COLORS = 4;
    static const glm::vec2 COLOR_MARGIN = glm::vec2(0.025f);
    static const float COLOR_SCALE = 0.1f;

    ColorDisplaySubEntity::ColorDisplaySubEntity(UITransform containerTransform,
        UITransform* rootTransform)
    {
        container = std::make_unique<UISubEntity>(containerTransform);
        rootTransform->BindChild(container->GetTransform());

        containerTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        containerMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        containerMaterial->SetTexture("_SpriteTex", containerTexture.get());
        containerMaterial->SetColor("_TintColor", glm::vec4(1));
        containerMaterial->SetVector("_Scale", glm::vec4(1));

        colorEntities.reserve(NUM_COLORS);
        colorMaterials.reserve(NUM_COLORS);

        colorTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_CircleSprite");

        UITransform colorTransform{};
        colorTransform.Scale = glm::vec2(COLOR_SCALE);
        colorTransform.Position = COLOR_MARGIN;
        colorTransform.Anchor = AnchorMode::BottomLeft;
        colorTransform.Pivot = AnchorMode::BottomLeft;
        colorTransform.Depth = containerTransform.Depth + 0.05f;

        for (int i = 0; i < NUM_COLORS; i++)
        {
            auto colorEntity = std::make_unique<UISubEntity>(colorTransform);
            container->GetTransform()->BindChild(colorEntity->GetTransform());

            colorEntities.push_back(std::move(colorEntity));
            colorTransform.Position += glm::vec2(0, COLOR_SCALE);

            auto colorMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
            colorMaterial->SetTexture("_SpriteTex", colorTexture.get());
            colorMaterial->SetColor("_TintColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0));
            colorMaterial->SetVector("_Scale", glm::vec4(1));

            colorMaterials.push_back(std::move(colorMaterial));
        }
    }
} // namespace Beer::System
