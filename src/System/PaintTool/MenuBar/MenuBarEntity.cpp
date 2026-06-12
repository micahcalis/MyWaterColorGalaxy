#include "System/PaintTool/MenuBar/MenuBarEntity.hpp"
#include "MenuBarManager.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include <memory>

namespace Beer::System
{
    static const float MENU_HEIGHT = 0.8f;
    static const float MENU_WIDTH = 0.25f;
    static const float MENU_OFFSET = 0.05f;
    static const float SEED_BUTTON_SIZE = 0.2f;
    static const float FLY_BUTTON_SIZE = 0.2f;
    static const float BUTTON_PADDING = 0.05f;

    MenuBarEntity::MenuBarEntity(GalaxyMapBuffer* galaxyMapBuffer,
        Function<void> clearHistory,
        Function<void> saveMap,
        Function<void> onBackToTitle,
        Function<void> enableBlock)
        : galaxyMapBuffer(galaxyMapBuffer)
        , clearHistory(clearHistory)
        , saveMap(saveMap)
        , onBackToTitle(onBackToTitle)
        , enableBlock(enableBlock)
        , QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
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

        seedButtonTexture = std::make_shared<Rendering::Texture2D>("UI/MenuBar/Tex_NewSeed");
        seedButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        seedButtonMaterial->SetColor("_TintColor", glm::vec4(1.0f));
        seedButtonMaterial->SetVector("_Scale", glm::vec4(1));
        seedButtonMaterial->SetTexture("_SpriteTex", seedButtonTexture.get());

        UITransform seedTransform{};
        seedTransform.Anchor = AnchorMode::TopMiddle;
        seedTransform.Pivot = AnchorMode::TopMiddle;
        seedTransform.Scale = glm::vec2(SEED_BUTTON_SIZE);
        seedTransform.Position = glm::vec2(0, -BUTTON_PADDING);

        seedButtonEntity = std::make_unique<UISubEntity>(seedTransform);
        rootTransform.BindChild(seedButtonEntity->GetTransform());

        UITransform flyTransform{};
        flyTransform.Anchor = AnchorMode::TopMiddle;
        flyTransform.Pivot = AnchorMode::TopMiddle;
        flyTransform.Scale = glm::vec2(SEED_BUTTON_SIZE);
        flyTransform.Position = glm::vec2(0, -BUTTON_PADDING - SEED_BUTTON_SIZE);

        flyButtonTexture = std::make_shared<Rendering::Texture2D>("UI/MenuBar/Tex_Fly");
        flyButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        flyButtonMaterial->SetColor("_TintColor", glm::vec4(1.0f));
        flyButtonMaterial->SetVector("_Scale", glm::vec4(1));
        flyButtonMaterial->SetTexture("_SpriteTex", flyButtonTexture.get());

        flyButtonEntity = std::make_unique<UISubEntity>(flyTransform);
        rootTransform.BindChild(flyButtonEntity->GetTransform());

        UITransform backTransform{};
        backTransform.Anchor = AnchorMode::TopMiddle;
        backTransform.Pivot = AnchorMode::TopMiddle;
        backTransform.Scale = glm::vec2(SEED_BUTTON_SIZE);
        backTransform.Position = glm::vec2(0, -BUTTON_PADDING - (SEED_BUTTON_SIZE * 2.0f));

        backButtonTexture = std::make_shared<Rendering::Texture2D>("UI/MenuBar/Tex_Back");
        backButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backButtonMaterial->SetColor("_TintColor", glm::vec4(1.0f));
        backButtonMaterial->SetVector("_Scale", glm::vec4(1));
        backButtonMaterial->SetTexture("_SpriteTex", backButtonTexture.get());

        backButtonEntity = std::make_unique<UISubEntity>(backTransform);
        rootTransform.BindChild(backButtonEntity->GetTransform());

        menuBarManager->InitializeButtons(seedButtonEntity->GetTransform(),
            seedButtonMaterial.get(),
            flyButtonEntity->GetTransform(),
            flyButtonMaterial.get(),
            backButtonEntity->GetTransform(),
            backButtonMaterial.get());

        MarkDirty();
    }
} // namespace Beer::System
