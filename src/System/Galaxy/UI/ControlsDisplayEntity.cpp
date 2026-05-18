#include "System/Galaxy/UI/ControlsDisplayEntity.hpp"
#include "ControlsDisplayEntity.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::System
{
    static const glm::vec2 CONTROL_DISPLAY_SIZE = glm::vec2(0.3f, 0.3f);
    static const glm::vec2 CONTROL_DISPLAY_PADDING = glm::vec2(0.05f, 0.05f);

    ControlsDisplayEntity::ControlsDisplayEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::GalaxyUserInt))
    {
        rootTransform.Anchor = AnchorMode::BottomLeft;
        rootTransform.Pivot = AnchorMode::BottomLeft;
        rootTransform.Scale = glm::vec2(CONTROL_DISPLAY_SIZE.x * 2.0f + CONTROL_DISPLAY_PADDING.x * 3.0f,
            CONTROL_DISPLAY_SIZE.y + CONTROL_DISPLAY_PADDING.y * 2.0f);

        InitializeDisplays();
    }

    void ControlsDisplayEntity::InitializeDisplays()
    {
        UITransform displayTransform{};
        displayTransform.Anchor = AnchorMode::BottomLeft;
        displayTransform.Pivot = AnchorMode::BottomLeft;
        displayTransform.Scale = CONTROL_DISPLAY_SIZE;
        displayTransform.Position = CONTROL_DISPLAY_PADDING;

        boostDisplayEntity = std::make_unique<UISubEntity>(displayTransform);
        rootTransform.BindChild(boostDisplayEntity->GetTransform());

        boostDisplayTexture = std::make_shared<Rendering::Texture2D>("UI/ControlsDisplay/Tex_BoostDisplay");
        boostDisplayMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        boostDisplayMaterial->SetColor("_TintColor", glm::vec4(1));
        boostDisplayMaterial->SetVector("_Scale", glm::vec4(1));
        boostDisplayMaterial->SetTexture("_SpriteTex", boostDisplayTexture.get());

        displayTransform.Anchor = AnchorMode::BottomRight;
        displayTransform.Pivot = AnchorMode::BottomRight;
        displayTransform.Position = glm::vec2(-CONTROL_DISPLAY_PADDING.x, CONTROL_DISPLAY_PADDING.y);

        steerDisplayEntity = std::make_unique<UISubEntity>(displayTransform);
        rootTransform.BindChild(steerDisplayEntity->GetTransform());

        steerDisplayTexture = std::make_shared<Rendering::Texture2D>("UI/ControlsDisplay/Tex_SteerDisplay");
        steerDisplayMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        steerDisplayMaterial->SetColor("_TintColor", glm::vec4(1));
        steerDisplayMaterial->SetVector("_Scale", glm::vec4(1));
        steerDisplayMaterial->SetTexture("_SpriteTex", steerDisplayTexture.get());
    }
} // namespace Beer::System
