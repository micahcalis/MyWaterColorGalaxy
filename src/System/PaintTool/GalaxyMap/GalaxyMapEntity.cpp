#include "System/PaintTool/GalaxyMap/GalaxyMapEntity.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "GalaxyComponent.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapManager.hpp"

namespace Beer::System
{
    static const float MAP_SCALE = 2.0f;
    static const float STARS_FREQUENCY = 10.0f;
    static const float STARS_SCALE = 0.2f;
    static const glm::vec4 STARS_COLOR = glm::vec4(0.97f, 0.97f, 0.7f, 1.0f);
    static const float STARS_GLOW = 10.0f;
    static const float STARS_FLICKER_SPEED = 1.0f;
    static const float DUST_FREQUENCY = 5.0f;
    static const float DUST_THRESHOLD = 0.5f;
    static const float DUST_EDGE_THICKNESS = 0.025f;
    static const int DUST_GRAD_STEPS = 9;
    static const float DUST_OPACITY = 0.2f;
    static const int RINGS_NUMBER = 7;
    static const float RINGS_THICKNESS = 0.15f;
    static const glm::vec4 RINGS_COLOR = glm::vec4(0.9f, 0.9f, 0.8f, 1.0f);
    static const float CENTER_STAR_SCALE = 0.15f;
    static const float CENTER_STAR_GRAD_INTENSITY = 2.0f;
    static const float EDGE_RING_INTENSITY = 0.5f;
    static const int CENTER_STAR_GRAD_STEPS = 6;

    GalaxyMapEntity::GalaxyMapEntity(GalaxyMapBuffer* galaxyMapBuffer,
        Function<MouseInput> getMouseInput,
        Function<bool> isColorMixerOpen)
        : galaxyMapBuffer(galaxyMapBuffer), getMouseInput(getMouseInput), isColorMixerOpen(isColorMixerOpen), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
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

        InitializePerlinWorleyTex();
        InitializeStar();
        InitializePerlinTex();
        MarkDirty();
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
    }
} // namespace Beer::System
