#include "System/PaintTool/GalaxyMap/GalaxyMapEntity.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "GalaxyComponent.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "SDL3/SDL_gpu.h"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapManager.hpp"

namespace Beer::System
{
    static const float MAP_SCALE = 1.925f;
    static const glm::vec2 MAP_OFFSET = glm::vec2(0.25f, 0.02f);
    static const glm::vec2 FRAME_SCALE = glm::vec2(2.2f * 1.19f, 2.2f);
    static const glm::vec2 FRAME_OFFSET = glm::vec2(0.1375f, -0.02f);
    static const float STARS_FREQUENCY = 10.0f;
    static const float STARS_SCALE = 0.2f;
    static const glm::vec4 STARS_COLOR = glm::vec4(0.97f, 0.97f, 0.7f, 1.0f);
    static const float STARS_GLOW = 15.0f;
    static const float STARS_FLICKER_SPEED = 1.0f;
    static const float DUST_FREQUENCY = 5.0f;
    static const float DUST_THRESHOLD = 0.5f;
    static const float DUST_EDGE_THICKNESS = 0.025f;
    static const int DUST_GRAD_STEPS = 9;
    static const float DUST_OPACITY = 0.2f;
    static const int RINGS_NUMBER = 7;
    static const float RINGS_THICKNESS = 0.15f;
    static const glm::vec4 RINGS_COLOR = glm::vec4(0.9f, 0.9f, 0.8f, 1.0f);
    static const float CENTER_STAR_GRAD_INTENSITY = 2.0f;
    static const float EDGE_RING_INTENSITY = 0.5f;
    static const int CENTER_STAR_GRAD_STEPS = 6;
    static const float PLAYER_INDICATOR_SCALE = 0.1f;
    static const float ZOOM_INDICATOR_SCALE = 0.2f;
    static const glm::vec2 ZOOM_INDICATOR_OFFSET = glm::vec2(0.05f, 0);

    static const glm::vec2 HELP_BUTTON_SCALE = glm::vec2(0.1f);
    static const glm::vec2 HELP_BUTTON_OFFSET = glm::vec2(-0.25f, 0.3f);
    static const glm::vec2 HELP_POPUP_SCALE = glm::vec2(0.7f, 0.43f);
    static const glm::vec2 HELP_POPUP_OFFSET = glm::vec2(0.0f, 0.05f);
    static const std::string HELP_TEXT = "This is your Galaxy Map! Here you can place and erase all your galaxy objects. Want to start over? Select ‘New’, which will replace your current map with a new empty map. Ready to explore your galaxy? Press ‘Fly’ to enter your generated creation!";

    GalaxyMapEntity::GalaxyMapEntity(GalaxyMapBuffer* galaxyMapBuffer,
        Function<MouseInput> getMouseInput)
        : galaxyMapBuffer(galaxyMapBuffer), getMouseInput(getMouseInput), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        galaxyMaterial = std::make_shared<Rendering::Material>("UI/GalaxyMapSprite");
        galaxyMapBuffer->SetMapMaterial(galaxyMaterial);
        galaxyMaterial->SetFloat("_StarFrequency", STARS_FREQUENCY);
        galaxyMaterial->SetFloat("_StarScale", STARS_SCALE);
        galaxyMaterial->SetColor("_StarColor", STARS_COLOR);
        galaxyMaterial->SetFloat("_StarGlowIntensity", STARS_GLOW);
        galaxyMaterial->SetFloat("_StarFlickerSpeed", STARS_FLICKER_SPEED);
        galaxyMaterial->SetFloat("_DustFrequency", DUST_FREQUENCY);
        galaxyMaterial->SetFloat("_DustThreshold", DUST_THRESHOLD);
        galaxyMaterial->SetFloat("_DustEdgeThickness", DUST_EDGE_THICKNESS);
        galaxyMaterial->SetInt("__DustGradientSteps", DUST_GRAD_STEPS);
        galaxyMaterial->SetFloat("_DustOpacity", DUST_OPACITY);
        galaxyMaterial->SetInt("_RingCount", RINGS_NUMBER);
        galaxyMaterial->SetFloat("_RingThickness", RINGS_THICKNESS);
        galaxyMaterial->SetColor("_RingColor", RINGS_COLOR);

        rootTransform.Anchor = AnchorMode::Center;
        rootTransform.Pivot = AnchorMode::Center;
        rootTransform.Scale = glm::vec2(MAP_SCALE);
        rootTransform.Position = MAP_OFFSET;
        rootTransform.Depth = 0.01f;

        InitializeFrame();
        InitializePerlinWorleyTex();
        InitializeStar();
        InitializePerlinTex();
        InitializePlayerIndicator();
        MarkDirty();
    }

    void GalaxyMapEntity::InitializeFrame()
    {
        UITransform frameTransform{};
        frameTransform.Anchor = AnchorMode::Center;
        frameTransform.Pivot = AnchorMode::Center;
        frameTransform.Scale = FRAME_SCALE;
        frameTransform.Position = FRAME_OFFSET;
        frameTransform.Depth = 0.1f;

        frameEntity = std::make_unique<UISubEntity>(frameTransform);
        rootTransform.BindChild(frameEntity->GetTransform());

        frameTexture = std::make_shared<Rendering::Texture2D>("UI/GalaxyMap/Tex_MapFrame");
        frameMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        frameMaterial->SetTexture("_SpriteTex", frameTexture.get());
        frameMaterial->SetColor("_TintColor", glm::vec4(1));
        frameMaterial->SetVector("_Scale", glm::vec4(1));
    }

    void GalaxyMapEntity::InitializePerlinWorleyTex()
    {
        Rendering::TextureMakeSettings makeSettings{};
        makeSettings.Width = 1024;
        makeSettings.Height = 1024;
        makeSettings.KernelIndex = 0;

        generatePerlinWorleyContext = std::make_shared<Rendering::ComputeContext>("Texture/ComputeNoise");
        generatePerlinWorleyContext->SetInt("_Depth", 6);
        generatePerlinWorleyContext->SetInt("_Seed", 5);
        generatePerlinWorleyContext->SetFloat("_Frequency", 2.5f);
        generatePerlinWorleyContext->SetFloat("_FrequencyMultiplier", 2.0f);
        generatePerlinWorleyContext->SetFloat("_AmplitudeMultiplier", 0.9f);
        generatePerlinWorleyContext->SetFloat("_NoiseBalance", 0.55f);
        generatePerlinWorleyContext->SetFloat("_Exponent", 2.0f);
        generatePerlinWorleyContext->SetFloat("_Edge", 0.6f);
        generatePerlinWorleyContext->Update();

        perlinWorleyTexture = std::make_shared<Rendering::Texture2D>(Rendering::Texture2D::Make(
            makeSettings,
            generatePerlinWorleyContext.get()));

        galaxyMaterial->SetTexture("_NoiseTexture", perlinWorleyTexture.get());
    }

    void GalaxyMapEntity::InitializeStar()
    {
        starMaterial = std::make_shared<Rendering::Material>("UI/SunSprite");
        starMaterial->SetFloat("_GradientIntensity", CENTER_STAR_GRAD_INTENSITY);
        starMaterial->SetFloat("_EdgeRingIntensity", EDGE_RING_INTENSITY);
        starMaterial->SetInt("_GradientSteps", CENTER_STAR_GRAD_STEPS);

        UITransform starTransform{};
        starTransform.Anchor = AnchorMode::BottomLeft;
        starTransform.Pivot = AnchorMode::Center;
        starTransform.Scale = glm::vec2(CENTER_STAR_SCALE);
        starTransform.Depth = 0.02f;

        starEntity = std::make_unique<UISubEntity>(starTransform);
        rootTransform.BindChild(starEntity->GetTransform());

        galaxyMapBuffer->SetStarMaterial(starMaterial,
            [this](glm::vec2 pos) -> void { UpdateStarPosition(pos); });
    }

    void GalaxyMapEntity::InitializePerlinTex()
    {
        Rendering::TextureMakeSettings makeSettings{};
        makeSettings.Width = 512;
        makeSettings.Height = 512;
        makeSettings.KernelIndex = 0;

        generatePerlinContext = std::make_shared<Rendering::ComputeContext>("Texture/ComputeNoise");
        generatePerlinContext->SetInt("_Depth", 3);
        generatePerlinContext->SetInt("_Seed", 5);
        generatePerlinContext->SetFloat("_Frequency", 2.5f);
        generatePerlinContext->SetFloat("_FrequencyMultiplier", 2.0f);
        generatePerlinContext->SetFloat("_AmplitudeMultiplier", 0.9f);
        generatePerlinContext->SetFloat("_NoiseBalance", 0.55f);
        generatePerlinContext->SetFloat("_Exponent", 2.0f);
        generatePerlinContext->SetFloat("_Edge", 0.6f);
        generatePerlinContext->Update();

        perlinTexture = std::make_shared<Rendering::Texture2D>(Rendering::Texture2D::Make(
            makeSettings,
            generatePerlinContext.get()));

        starMaterial->SetTexture("_NoiseTexture", perlinTexture.get());
    }

    void GalaxyMapEntity::UpdateStarPosition(glm::vec2 position)
    {
        glm::vec2 scaledPosition = position * rootTransform.Scale;
        starEntity->GetTransform()->Position = scaledPosition;
        MarkDirty();
    }

    void GalaxyMapEntity::InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor)
    {
        GalaxyMapManager* mapManager = GetMapManager();
        mapManager->InitializeCursor(getColor, starEntity->GetTransform());
        mapManager->GetCursor()->OnComponentPlaced.Subscribe([this](uint32_t, GalaxyComponentData, bool) -> void { MarkDirty(); });
        mapManager->GetCursor()->OnComponentErased.Subscribe([this](uint32_t, GalaxyComponentData, bool) -> void { MarkDirty(); });

        InitializeHelpButton();
    }

    void GalaxyMapEntity::InitializePlayerIndicator()
    {
        UITransform indicatorTransform{};
        indicatorTransform.Anchor = AnchorMode::BottomLeft;
        indicatorTransform.Pivot = AnchorMode::Center;
        indicatorTransform.Scale = glm::vec2(PLAYER_INDICATOR_SCALE);
        indicatorTransform.Depth = 0.1f;

        playerIndicatorEntity = std::make_unique<UISubEntity>(indicatorTransform);
        rootTransform.BindChild(playerIndicatorEntity->GetTransform());

        playerIndicatorTexture = std::make_shared<Rendering::Texture2D>("UI/GalaxyMap/Tex_PlayerIndicator");
        playerIndicatorMaterial = std::make_shared<Rendering::Material>("UI/PlayerIndicatorSprite");
        playerIndicatorMaterial->SetTexture("_SpriteTex", playerIndicatorTexture.get());
        playerIndicatorMaterial->SetColor("_TintColor", glm::vec4(1));
    }

    void GalaxyMapEntity::InitializeHelpButton()
    {
        UITransform helpButtonTransform{};
        helpButtonTransform.Anchor = AnchorMode::MiddleRight;
        helpButtonTransform.Pivot = AnchorMode::MiddleLeft;
        helpButtonTransform.Scale = HELP_BUTTON_SCALE;
        helpButtonTransform.Position = HELP_BUTTON_OFFSET;
        helpButtonTransform.Depth = 0.5f;

        UITransform helpPopupTransform{};
        helpPopupTransform.Anchor = AnchorMode::Center;
        helpPopupTransform.Pivot = AnchorMode::Center;
        helpPopupTransform.Scale = HELP_POPUP_SCALE;
        helpPopupTransform.Position = HELP_POPUP_OFFSET;
        helpPopupTransform.Depth = 0.6f;

        helpButtonSubEntity = std::make_unique<HelpButtonSubEntity>(
            frameEntity->GetTransform(),
            helpButtonTransform,
            HELP_TEXT,
            helpPopupTransform);

        GetMapManager()->SetHelpToggle(helpButtonSubEntity.get(),
            [this]() -> void { MarkDirty(); });
    }
} // namespace Beer::System
