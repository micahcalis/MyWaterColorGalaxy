#include "System/Galaxy/UI/OptionsDisplayEntity.hpp"
#include "OptionsDisplayEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    static const glm::vec2 OPTION_DISPLAY_SIZE = glm::vec2(0.3f, 0.3f);
    static const glm::vec2 OPTION_DISPLAY_PADDING = glm::vec2(0.05f, 0.05f);

    OptionsDisplayEntity::OptionsDisplayEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::GalaxyUserInt))
    {
        rootTransform.Anchor = AnchorMode::BottomRight;
        rootTransform.Pivot = AnchorMode::BottomRight;
        rootTransform.Scale = glm::vec2(OPTION_DISPLAY_SIZE.x * 2.0f + OPTION_DISPLAY_PADDING.x * 3.0f,
            OPTION_DISPLAY_SIZE.y + OPTION_DISPLAY_PADDING.y * 2.0f);

        InitializeDisplays();
    }

    void OptionsDisplayEntity::InitializeDisplays()
    {
        UITransform displayTransform{};
        displayTransform.Anchor = AnchorMode::BottomLeft;
        displayTransform.Pivot = AnchorMode::BottomLeft;
        displayTransform.Scale = OPTION_DISPLAY_SIZE;
        displayTransform.Position = OPTION_DISPLAY_PADDING;

        returnDisplayEntity = std::make_unique<UISubEntity>(displayTransform);
        rootTransform.BindChild(returnDisplayEntity->GetTransform());

        returnDisplayTexture = std::make_shared<Rendering::Texture2D>("UI/OptionsDisplay/Tex_ReturnDisplay");
        returnDisplayMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        returnDisplayMaterial->SetColor("_TintColor", glm::vec4(1));
        returnDisplayMaterial->SetVector("_Scale", glm::vec4(1));
        returnDisplayMaterial->SetTexture("_SpriteTex", returnDisplayTexture.get());

        displayTransform.Anchor = AnchorMode::BottomRight;
        displayTransform.Pivot = AnchorMode::BottomRight;
        displayTransform.Position = glm::vec2(-OPTION_DISPLAY_PADDING.x, OPTION_DISPLAY_PADDING.y);

        photoDisplayEntity = std::make_unique<UISubEntity>(displayTransform);
        rootTransform.BindChild(photoDisplayEntity->GetTransform());

        photoDisplayTexture = std::make_shared<Rendering::Texture2D>("UI/OptionsDisplay/Tex_PhotoDisplay");
        photoDisplayMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        photoDisplayMaterial->SetColor("_TintColor", glm::vec4(1));
        photoDisplayMaterial->SetVector("_Scale", glm::vec4(1));
        photoDisplayMaterial->SetTexture("_SpriteTex", photoDisplayTexture.get());
    }

} // namespace Beer::System
