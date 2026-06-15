#include "System/Galaxy/UI/ExitPhotoModeEntity.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    static const glm::vec2 EXIT_PHOTOMODE_SIZE = glm::vec2(0.2f, 0.2f);
    static const glm::vec2 EXIT_PHOTOMODE_PADDING = glm::vec2(-0.05f, 0.05f);
    static const glm::vec4 EXIT_PHOTOMODE_COLOR = glm::vec4(1, 1, 1, 0.5f);

    ExitPhotoModeEntity::ExitPhotoModeEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::GalaxyUserInt))
    {
        rootTransform.Anchor = AnchorMode::BottomRight;
        rootTransform.Pivot = AnchorMode::BottomRight;

        UITransform exitPhotoModeTransform{};
        exitPhotoModeTransform.Anchor = AnchorMode::BottomRight;
        exitPhotoModeTransform.Pivot = AnchorMode::BottomRight;
        exitPhotoModeTransform.Scale = EXIT_PHOTOMODE_SIZE;
        exitPhotoModeTransform.Position = EXIT_PHOTOMODE_PADDING;

        exitPhotoModeEntity = std::make_unique<UISubEntity>(exitPhotoModeTransform);
        rootTransform.BindChild(exitPhotoModeEntity->GetTransform());

        exitPhotoModeSprite = std::make_shared<Rendering::Texture2D>("UI/OptionsDisplay/Tex_PhotoDisplay");
        exitPhotModeMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        exitPhotModeMaterial->SetTexture("_SpriteTex", exitPhotoModeSprite.get());
        exitPhotModeMaterial->SetColor("_TintColor", EXIT_PHOTOMODE_COLOR);
        exitPhotModeMaterial->SetVector("_Scale", glm::vec4(1));
    }
} // namespace Beer::System
