#include "System/PaintTool/Background/BackgroundEntity.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    static const float BACKGROUND_SCALE = 8.0f;

    BackgroundEntity::BackgroundEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        rootTransform.Anchor = AnchorMode::BottomLeft;
        rootTransform.Anchor = AnchorMode::BottomLeft;
        rootTransform.Scale = glm::vec2(20);
        rootTransform.Depth = -0.1f;

        backgroundTexture = std::make_shared<Rendering::Texture2D>("UI/Background/Tex_ToolBackground");
        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetTexture("_SpriteTex", backgroundTexture.get());
        backgroundMaterial->SetColor("_TintColor", glm::vec4(1));
        backgroundMaterial->SetVector("_Scale", glm::vec4(BACKGROUND_SCALE));
    }
} // namespace Beer::System
