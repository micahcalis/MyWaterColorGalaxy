#include "System/Menus/UI/MainMenuEntity.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"

namespace Beer::System
{
    static const glm::vec2 LOGO_SIZE = glm::vec2(1.5f, 0.375);
    static const glm::vec2 LOGO_OFFSET = glm::vec2(0, 0.3f);

    static const glm::vec2 START_SIZE = glm::vec2(0.5f, 0.25f);
    static const glm::vec2 START_OFFSET = glm::vec2(0, -0.1f);

    static const glm::vec2 EXIT_SIZE = glm::vec2(0.4f, 0.2f);
    static const glm::vec2 EXIT_OFFSET = glm::vec2(0, -0.4f);

    MainMenuEntity::MainMenuEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::MainMenu))
    {
        rootTransform.Anchor = AnchorMode::Center;
        rootTransform.Pivot = AnchorMode::Center;

        logoTexture = std::make_shared<Rendering::Texture2D>("UI/TitleScreen/Tex_MwgLogo");
        logoMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        logoMaterial->SetTexture("_SpriteTex", logoTexture.get());
        logoMaterial->SetColor("_TintColor", glm::vec4(1));
        logoMaterial->SetVector("_Scale", glm::vec4(1));

        UITransform menuTransform{};
        menuTransform.Anchor = AnchorMode::Center;
        menuTransform.Pivot = AnchorMode::Center;
        menuTransform.Scale = LOGO_SIZE;
        menuTransform.Position = LOGO_OFFSET;
        logoEntity = std::make_unique<UISubEntity>(menuTransform);
        rootTransform.BindChild(logoEntity->GetTransform());

        startTexture = std::make_shared<Rendering::Texture2D>("UI/TitleScreen/Tex_StartButton");
        startMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        startMaterial->SetTexture("_SpriteTex", startTexture.get());
        startMaterial->SetColor("_TintColor", glm::vec4(1));
        startMaterial->SetVector("_Scale", glm::vec4(1));

        menuTransform.Scale = START_SIZE;
        menuTransform.Position = START_OFFSET;
        startEntity = std::make_unique<UISubEntity>(menuTransform);
        rootTransform.BindChild(startEntity->GetTransform());

        exitTexture = std::make_shared<Rendering::Texture2D>("UI/TitleScreen/Tex_ExitButton");
        exitMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        exitMaterial->SetTexture("_SpriteTex", exitTexture.get());
        exitMaterial->SetColor("_TintColor", glm::vec4(1));
        exitMaterial->SetVector("_Scale", glm::vec4(1));

        menuTransform.Scale = EXIT_SIZE;
        menuTransform.Position = EXIT_OFFSET;
        exitEntity = std::make_unique<UISubEntity>(menuTransform);
        rootTransform.BindChild(exitEntity->GetTransform());

        MarkDirty();
    }

    void MainMenuEntity::InitializeButtons()
    {
        GetMainMenuManager()->InitializeButtons(startEntity->GetTransform(),
            startMaterial.get(),
            exitEntity->GetTransform(),
            exitMaterial.get());
    }
} // namespace Beer::System
