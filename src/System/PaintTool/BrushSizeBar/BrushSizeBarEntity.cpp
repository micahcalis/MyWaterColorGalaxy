#include "System/PaintTool/BrushSizeBar/BrushSizeBarEntity.hpp"
#include "BrushSizeBarManager.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::System
{
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.25f, 0.8f);
    static const glm::vec2 PANEL_OFFSET = glm::vec2(0.6f, -0.02f);

    static const glm::vec2 SLIDER_CONTAINER_SIZE = glm::vec2(0.1f, 0.4f);
    static const glm::vec2 SLIDER_BUTTON_SIZE = glm::vec2(0.1f, 0.05f);
    static const glm::vec2 SMALL_ICON_SIZE = glm::vec2(0.05f, 0.05f);
    static const glm::vec2 BIG_ICON_SIZE = glm::vec2(0.085f, 0.085f);
    static const float ICON_OFFSET = 0.02f;

    BrushSizeBarEntity::BrushSizeBarEntity(Function<void, float> setBrushSize)
        : setBrushSize(setBrushSize), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        squareTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");

        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        backgroundMaterial->SetTexture("_SpriteTex", squareTexture.get());
        backgroundMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        rootTransform.Anchor = AnchorMode::TopLeft;
        rootTransform.Pivot = AnchorMode::TopLeft;
        rootTransform.Scale = PANEL_SIZE;
        rootTransform.Position = PANEL_OFFSET;

        MarkDirty();
    }

    void BrushSizeBarEntity::InitializeSlider()
    {
        BrushSizeBarManager* brushSizeBarManager = GetBrushSizeBarManager();

        UITransform sliderColliderTransform{};
        sliderColliderTransform.Anchor = AnchorMode::Center;
        sliderColliderTransform.Pivot = AnchorMode::Center;
        sliderColliderTransform.Scale = SLIDER_CONTAINER_SIZE;

        sliderColliderEntity = std::make_unique<UISubEntity>(sliderColliderTransform);
        rootTransform.BindChild(sliderColliderEntity->GetTransform());

        sliderColliderTransform.Scale.x *= 0.2f;
        sliderColliderTransform.Scale.y *= 1.2f;

        sliderBarEntity = std::make_unique<UISubEntity>(sliderColliderTransform);
        rootTransform.BindChild(sliderBarEntity->GetTransform());

        sliderBarMaterial = std::make_unique<Rendering::Material>("UI/SpriteDefault");
        sliderBarMaterial->SetColor("_TintColor", glm::vec4(0.2f, 0.2f, 0.2f, 1));
        sliderBarMaterial->SetVector("_Scale", glm::vec4(1));
        sliderBarMaterial->SetTexture("_SpriteTex", squareTexture.get());

        UITransform sliderButtonTransform{};
        sliderButtonTransform.Anchor = AnchorMode::Center;
        sliderButtonTransform.Pivot = AnchorMode::Center;
        sliderButtonTransform.Scale = SLIDER_BUTTON_SIZE;
        sliderButtonTransform.Depth += 0.1f;

        sliderButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        sliderButtonMaterial->SetColor("_TintColor", glm::vec4(0.4f, 0.4f, 0.4f, 1));
        sliderButtonMaterial->SetVector("_Scale", glm::vec4(1));
        sliderButtonMaterial->SetTexture("_SpriteTex", squareTexture.get());

        sliderButtonEntity = std::make_unique<UISubEntity>(sliderButtonTransform);

        brushSizeTexture = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_PlanetBrushSize");
        sizeIconMaterial = std::make_unique<Rendering::Material>("UI/SpriteDefault");
        sizeIconMaterial->SetColor("_TintColor", glm::vec4(1));
        sizeIconMaterial->SetVector("_Scale", glm::vec4(1));
        sizeIconMaterial->SetTexture("_SpriteTex", brushSizeTexture.get());

        UITransform sliderIconTransform{};
        sliderIconTransform.Anchor = AnchorMode::TopMiddle;
        sliderIconTransform.Pivot = AnchorMode::BottomMiddle;
        sliderIconTransform.Scale = SMALL_ICON_SIZE;
        sliderIconTransform.Position.y = ICON_OFFSET;

        smallSizeEntity = std::make_unique<UISubEntity>(sliderIconTransform);
        sliderColliderEntity->GetTransform()->BindChild(smallSizeEntity->GetTransform());

        sliderIconTransform.Anchor = AnchorMode::BottomMiddle;
        sliderIconTransform.Pivot = AnchorMode::TopMiddle;
        sliderIconTransform.Scale = BIG_ICON_SIZE;
        sliderIconTransform.Position.y = -ICON_OFFSET;

        bigSizeEntity = std::make_unique<UISubEntity>(sliderIconTransform);
        sliderColliderEntity->GetTransform()->BindChild(bigSizeEntity->GetTransform());

        brushSizeBarManager->InitializeSlider(sliderColliderEntity->GetTransform(),
            sliderButtonMaterial.get(),
            sliderButtonEntity->GetTransform());

        MarkDirty();
    }
} // namespace Beer::System
