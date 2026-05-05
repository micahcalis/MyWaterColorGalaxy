#include "System/PaintTool/MenuBar/MenuBarEntity.hpp"
#include "MenuBarManager.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"

namespace Beer::System
{
    static const float MENU_HEIGHT = 0.8f;
    static const float MENU_WIDTH = 0.25f;
    static const float MENU_OFFSET = 0.05f;
    static const float SEED_BUTTON_SIZE = 0.2f;
    static const float BUTTON_PADDING = 0.05f;

    MenuBarEntity::MenuBarEntity(GalaxyMapBuffer* galaxyMapBuffer)
        : galaxyMapBuffer(galaxyMapBuffer), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        rootTransform.Anchor = AnchorMode::BottomRight;
        rootTransform.Pivot = AnchorMode::BottomRight;
        rootTransform.Scale = glm::vec2(MENU_WIDTH, MENU_HEIGHT);
        rootTransform.Position = glm::vec2(-MENU_OFFSET, MENU_OFFSET);

        backgroundTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetColor("_TintColor", glm::vec4(1));
        backgroundMaterial->SetVector("_Scale", glm::vec4(1));
        backgroundMaterial->SetTexture("_SpriteTex", backgroundTexture.get());

        MarkDirty();
    }

    void MenuBarEntity::InitializeButtonEntities()
    {
        MenuBarManager* menuBarManager = GetMenuBarManager();

        seedButtonTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        seedButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        seedButtonMaterial->SetColor("_TintColor", glm::vec4(0.1f, 0.25f, 0.8f, 1.0f));
        seedButtonMaterial->SetVector("_Scale", glm::vec4(1));
        seedButtonMaterial->SetTexture("_SpriteTex", seedButtonTexture.get());

        UITransform seedTransform{};
        seedTransform.Anchor = AnchorMode::TopMiddle;
        seedTransform.Pivot = AnchorMode::TopMiddle;
        seedTransform.Scale = glm::vec2(SEED_BUTTON_SIZE);
        seedTransform.Position = glm::vec2(0, -BUTTON_PADDING);

        seedButtonEntity = std::make_unique<UISubEntity>(seedTransform);
        rootTransform.BindChild(seedButtonEntity->GetTransform());

        menuBarManager->InitializeButtons(seedButtonEntity->GetTransform(), seedButtonMaterial.get());

        MarkDirty();
    }
} // namespace Beer::System
