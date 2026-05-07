#include "System/PaintTool/BrushSizeBar/BrushSizeBarEntity.hpp"
#include "BrushSizeBarManager.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.25f, 0.75f);
    static const glm::vec2 PANEL_OFFSET = glm::vec2(-0.05f, -0.05f);

    static const glm::vec2 SLIDER_CONTAINER_SIZE = glm::vec2(0.1f, 0.4f);
    static const glm::vec2 SLIDER_BUTTON_SIZE = glm::vec2(0.1f, 0.05f);

    BrushSizeBarEntity::BrushSizeBarEntity(Function<void, float> setBrushSize)
        : setBrushSize(setBrushSize), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        squareTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");

        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        backgroundMaterial->SetTexture("_SpriteTex", squareTexture.get());
        backgroundMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        rootTransform.Anchor = AnchorMode::TopRight;
        rootTransform.Pivot = AnchorMode::TopRight;
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

        UITransform sliderButtonTransform{};
        sliderButtonTransform.Anchor = AnchorMode::Center;
        sliderButtonTransform.Pivot = AnchorMode::Center;
        sliderButtonTransform.Scale = SLIDER_BUTTON_SIZE;

        sliderButtonMaterial = std::make_unique<Rendering::Material>("UI/SpriteDefault");
        sliderButtonMaterial->SetColor("_TintColor", glm::vec4(0.3f, 0.3f, 0.3f, 1));
        sliderButtonMaterial->SetVector("_Scale", glm::vec4(1));
        sliderButtonMaterial->SetTexture("_SpriteTex", squareTexture.get());

        sliderButtonEntity = std::make_unique<UISubEntity>(sliderButtonTransform);

        brushSizeBarManager->InitializeSlider(sliderColliderEntity->GetTransform(),
            sliderButtonMaterial.get(),
            sliderButtonEntity->GetTransform());

        MarkDirty();
    }
} // namespace Beer::System
