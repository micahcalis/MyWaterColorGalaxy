#include "System/PaintTool/MenuBar/MenuBarEntity.hpp"
#include "MenuBarManager.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include <memory>

namespace Beer::System
{
    static const float MENU_HEIGHT = 0.25f;
    static const float MENU_WIDTH = 0.8f;
    static const float MENU_OFFSET = 0.0f;
    static const float SEED_BUTTON_SIZE = 0.2f;
    static const float FLY_BUTTON_SIZE = 0.2f;
    static const float BUTTON_PADDING = 0.05f;

    MenuBarEntity::MenuBarEntity(GalaxyMapBuffer* galaxyMapBuffer,
        Function<void> clearHistory,
        Function<void> saveMap,
        Function<void> onBackToTitle,
        Function<void> enableBlock,
        Rendering::FadeState initialFadeState)
        : galaxyMapBuffer(galaxyMapBuffer)
        , clearHistory(clearHistory)
        , saveMap(saveMap)
        , onBackToTitle(onBackToTitle)
        , enableBlock(enableBlock)
        , initialFadeState(initialFadeState)
        , QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        rootTransform.Anchor = AnchorMode::TopLeft;
        rootTransform.Pivot = AnchorMode::TopLeft;
        rootTransform.Scale = glm::vec2(MENU_WIDTH, MENU_HEIGHT);
        rootTransform.Position = glm::vec2(-MENU_OFFSET, MENU_OFFSET);

        backgroundTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetColor("_TintColor", glm::vec4(1));
        backgroundMaterial->SetVector("_Scale", glm::vec4(1));
        backgroundMaterial->SetTexture("_SpriteTex", backgroundTexture.get());

        InitializeButtonEntities();

        MarkDirty();
    }

    void MenuBarEntity::InitializeButtons(UITransform* flyTransform, Rendering::Material* flyMaterial)
    {
        GetMenuBarManager()->InitializeButtons(seedButtonEntity->GetTransform(),
            seedButtonMaterial.get(),
            flyTransform,
            flyMaterial,
            backButtonEntity->GetTransform(),
            backButtonMaterial.get());
    }

    void MenuBarEntity::InitializeButtonEntities()
    {
        seedButtonTexture = std::make_shared<Rendering::Texture2D>("UI/MenuBar/Tex_NewSeed");
        seedButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        seedButtonMaterial->SetColor("_TintColor", glm::vec4(1.0f));
        seedButtonMaterial->SetVector("_Scale", glm::vec4(1));
        seedButtonMaterial->SetTexture("_SpriteTex", seedButtonTexture.get());

        UITransform seedTransform{};
        seedTransform.Anchor = AnchorMode::TopLeft;
        seedTransform.Pivot = AnchorMode::TopLeft;
        seedTransform.Scale = glm::vec2(SEED_BUTTON_SIZE);
        seedTransform.Position = glm::vec2(BUTTON_PADDING, -BUTTON_PADDING);

        seedButtonEntity = std::make_unique<UISubEntity>(seedTransform);
        rootTransform.BindChild(seedButtonEntity->GetTransform());

        UITransform backTransform{};
        backTransform.Anchor = AnchorMode::TopLeft;
        backTransform.Pivot = AnchorMode::TopLeft;
        backTransform.Scale = glm::vec2(SEED_BUTTON_SIZE);
        backTransform.Position = glm::vec2(SEED_BUTTON_SIZE + BUTTON_PADDING, -BUTTON_PADDING);

        backButtonTexture = std::make_shared<Rendering::Texture2D>("UI/MenuBar/Tex_Back");
        backButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backButtonMaterial->SetColor("_TintColor", glm::vec4(1.0f));
        backButtonMaterial->SetVector("_Scale", glm::vec4(1));
        backButtonMaterial->SetTexture("_SpriteTex", backButtonTexture.get());

        backButtonEntity = std::make_unique<UISubEntity>(backTransform);
        rootTransform.BindChild(backButtonEntity->GetTransform());

        MarkDirty();
    }
} // namespace Beer::System
