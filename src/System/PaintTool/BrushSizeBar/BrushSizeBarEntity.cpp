#include "System/PaintTool/BrushSizeBar/BrushSizeBarEntity.hpp"
#include "BrushSizeBarManager.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::System
{
    static const glm::vec2 PANEL_SIZE = glm::vec2(0.6f, 0.125f);
    static const glm::vec2 PANEL_OFFSET = glm::vec2(0, 0.55f);

    static const glm::vec2 SLIDER_CONTAINER_SIZE = glm::vec2(0.25f, 0.075f);
    static const glm::vec2 SLIDER_BUTTON_SIZE = glm::vec2(0.05f, 0.1f);
    static const glm::vec2 SMALL_ICON_SIZE = glm::vec2(0.085f, 0.085f);
    static const glm::vec2 BIG_ICON_SIZE = glm::vec2(0.085f * 1.75f, 0.085f);
    static const float ICON_OFFSET = 0.02f;

    BrushSizeBarEntity::BrushSizeBarEntity(Function<void, float> setBrushSize)
        : setBrushSize(setBrushSize), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        backgroundMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        rootTransform.Anchor = AnchorMode::BottomMiddle;
        rootTransform.Pivot = AnchorMode::BottomMiddle;
        rootTransform.Scale = PANEL_SIZE;
        rootTransform.Position = PANEL_OFFSET;
        rootTransform.Depth = 0.25f;

        MarkDirty();
    }

    void BrushSizeBarEntity::InitializeSlider(UITransform* parent)
    {
        BrushSizeBarManager* brushSizeBarManager = GetBrushSizeBarManager();
        parent->BindChild(&rootTransform);

        UITransform sliderColliderTransform{};
        sliderColliderTransform.Anchor = AnchorMode::Center;
        sliderColliderTransform.Pivot = AnchorMode::Center;
        sliderColliderTransform.Scale = SLIDER_CONTAINER_SIZE;

        sliderColliderEntity = std::make_unique<UISubEntity>(sliderColliderTransform);
        rootTransform.BindChild(sliderColliderEntity->GetTransform());

        sliderColliderTransform.Scale.x *= 1.1f;
        sliderColliderTransform.Scale.y *= 0.4f;

        sliderBarEntity = std::make_unique<UISubEntity>(sliderColliderTransform);
        rootTransform.BindChild(sliderBarEntity->GetTransform());

        sliderBarTexture = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_PlanetSlider");
        sliderBarMaterial = std::make_unique<Rendering::Material>("UI/SpriteDefault");
        sliderBarMaterial->SetColor("_TintColor", glm::vec4(1));
        sliderBarMaterial->SetVector("_Scale", glm::vec4(1));
        sliderBarMaterial->SetTexture("_SpriteTex", sliderBarTexture.get());

        UITransform sliderButtonTransform{};
        sliderButtonTransform.Anchor = AnchorMode::Center;
        sliderButtonTransform.Pivot = AnchorMode::Center;
        sliderButtonTransform.Scale = SLIDER_BUTTON_SIZE;
        sliderButtonTransform.Depth += 0.1f;

        sliderButtonTexture = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_SliderButton");
        sliderButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        sliderButtonMaterial->SetColor("_TintColor", glm::vec4(0.4f, 0.4f, 0.4f, 1));
        sliderButtonMaterial->SetVector("_Scale", glm::vec4(1));
        sliderButtonMaterial->SetTexture("_SpriteTex", sliderButtonTexture.get());

        sliderButtonEntity = std::make_unique<UISubEntity>(sliderButtonTransform);

        brushSizeTextureBig = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_PlanetBrushBig");
        sizeIconMaterialBig = std::make_unique<Rendering::Material>("UI/SpriteDefault");
        sizeIconMaterialBig->SetColor("_TintColor", glm::vec4(1));
        sizeIconMaterialBig->SetVector("_Scale", glm::vec4(1));
        sizeIconMaterialBig->SetTexture("_SpriteTex", brushSizeTextureBig.get());

        brushSizeTextureSmall = std::make_shared<Rendering::Texture2D>("UI/ToolBar/Tex_PlanetBrushSmall");
        sizeIconMaterialSmall = std::make_unique<Rendering::Material>("UI/SpriteDefault");
        sizeIconMaterialSmall->SetColor("_TintColor", glm::vec4(1));
        sizeIconMaterialSmall->SetVector("_Scale", glm::vec4(1));
        sizeIconMaterialSmall->SetTexture("_SpriteTex", brushSizeTextureSmall.get());

        UITransform sliderIconTransform{};
        sliderIconTransform.Anchor = AnchorMode::MiddleLeft;
        sliderIconTransform.Pivot = AnchorMode::MiddleRight;
        sliderIconTransform.Scale = SMALL_ICON_SIZE;
        sliderIconTransform.Position.x = -ICON_OFFSET;

        smallSizeEntity = std::make_unique<UISubEntity>(sliderIconTransform);
        sliderColliderEntity->GetTransform()->BindChild(smallSizeEntity->GetTransform());

        sliderIconTransform.Anchor = AnchorMode::MiddleRight;
        sliderIconTransform.Pivot = AnchorMode::MiddleLeft;
        sliderIconTransform.Scale = BIG_ICON_SIZE;
        sliderIconTransform.Position.x = ICON_OFFSET;

        bigSizeEntity = std::make_unique<UISubEntity>(sliderIconTransform);
        sliderColliderEntity->GetTransform()->BindChild(bigSizeEntity->GetTransform());

        brushSizeBarManager->InitializeSlider(sliderColliderEntity->GetTransform(),
            sliderButtonMaterial.get(),
            sliderButtonEntity->GetTransform());

        MarkDirty();
    }
} // namespace Beer::System
