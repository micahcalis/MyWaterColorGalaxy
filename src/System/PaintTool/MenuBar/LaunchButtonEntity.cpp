#include "System/PaintTool/MenuBar/LaunchButtonEntity.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    static const float FLY_CONTAINER_SIZE = 0.4f;
    static const glm::vec FLY_CONTAINER_OFFSET = glm::vec2(-0.1f, -0.45f);
    static const float FLY_BUTTON_SIZE = 0.25f;

    LaunchButtonEntity::LaunchButtonEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        rootTransform.Anchor = AnchorMode::TopRight;
        rootTransform.Pivot = AnchorMode::MiddleRight;
        rootTransform.Scale = glm::vec2(FLY_CONTAINER_SIZE);
        rootTransform.Position = FLY_CONTAINER_OFFSET;

        backgroundTexture = std::make_shared<Rendering::Texture2D>("UI/LaunchButton/Tex_LaunchButtonBackground");
        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetTexture("_SpriteTex", backgroundTexture.get());
        backgroundMaterial->SetColor("_TintColor", glm::vec4(1));
        backgroundMaterial->SetVector("_Scale", glm::vec4(1));

        UITransform flyButtonTransform{};
        flyButtonTransform.Anchor = AnchorMode::Center;
        flyButtonTransform.Pivot = AnchorMode::Center;
        flyButtonTransform.Scale = glm::vec2(FLY_BUTTON_SIZE);
        flyButtonTransform.Depth = rootTransform.Depth + 0.05F;

        flyButtonEntity = std::make_unique<UISubEntity>(flyButtonTransform);
        rootTransform.BindChild(flyButtonEntity->GetTransform());

        flyButtonTexture = std::make_shared<Rendering::Texture2D>("UI/LaunchButton/Tex_LaunchButton");
        flyButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        flyButtonMaterial->SetTexture("_SpriteTex", flyButtonTexture.get());
        flyButtonMaterial->SetColor("_TintColor", glm::vec4(1));
        flyButtonMaterial->SetVector("_Scale", glm::vec4(1));
    }
} // namespace Beer::System
