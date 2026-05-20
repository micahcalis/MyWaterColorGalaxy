#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Galaxy/Player/PlayerManager.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include <memory>

namespace Beer::System
{
    static const glm::vec4 SAUCER_COLOR = glm::vec4(0.75f, 0.0f, 0.0f, 1);
    static const float SAUCER_METALLIC = 1.0f;
    static const float SAUCER_SMOOTH = 0.8f;
    static const glm::vec4 COCKPIT_COLOR = glm::vec4(1.0f, 1.0f, 0.6f, 0.5f);
    static const float COCKPIT_METALLIC = 0.0f;
    static const float COCKPIT_SMOOTH = 0.6f;
    static const float PLAYER_SCALE = 1.5f;
    static const glm::vec4 CUTOFF_COLOR = glm::vec4(0.25f, 1.0f, 0.08f, 1.0f);
    static const float GLOW_THICKNESS = 0.1f;

    PlayerEntity::PlayerEntity(Function<PlayerInput> getPlayerInput,
        BeerEvent<void(bool)>* onSetPhotoMode,
        Layer layer)
        : getPlayerInput(getPlayerInput), onSetPhotoMode(onSetPhotoMode), GameEntity(layer)
    {
        transform.Position = PLAYER_SETTINGS.StartPos;
        transform.Scale = glm::vec3(PLAYER_SCALE);

        test3DContext = std::make_shared<Rendering::ComputeContext>("Texture/ComputeNoise3D");

        Rendering::TextureMakeSettings makeSettings{};
        makeSettings.Width = 128;
        makeSettings.Height = 128;
        makeSettings.Depth = 128;
        makeSettings.GroupSizeX = 8;
        makeSettings.GroupSizeY = 8;
        makeSettings.GroupSizeZ = 8;
        makeSettings.KernelIndex = 0;

        test3DTexture = std::make_shared<Rendering::Texture3D>(
            Rendering::Texture3D::Make(makeSettings, test3DContext.get()));

        playerMaterial = std::make_shared<Rendering::Material>("Galaxy/UFO");
        playerMaterial->SetColor("_SaucerColor", SAUCER_COLOR);
        playerMaterial->SetFloat("_SaucerMetallic", SAUCER_METALLIC);
        playerMaterial->SetFloat("_SaucerSmoothness", SAUCER_SMOOTH);
        playerMaterial->SetColor("_CockPitColor", COCKPIT_COLOR);
        playerMaterial->SetFloat("_CockPitMetallic", COCKPIT_METALLIC);
        playerMaterial->SetFloat("_CockPitSmoothness", COCKPIT_SMOOTH);
        playerMaterial->SetColor("_CutoffColor", CUTOFF_COLOR);
        playerMaterial->SetFloat("_GlowThickness", GLOW_THICKNESS);
        playerMaterial->SetFloat("_CutoffTime", 1.0f);
        playerMaterial->SetTexture("_Test3D", test3DTexture.get());

        playerMesh = Rendering::Mesh::Get("MDL_Cube");

        std::unique_ptr<SingleMeshRender> singleMeshRender
            = RenderRegister::CreateRenderComponent<SingleMeshRender>(
                ContextType::Galaxy,
                playerMaterial,
                playerMesh,
                &transform,
                &layer);

        this->renderComponent = (std::move(singleMeshRender));
    }

    void PlayerEntity::Update()
    {
        manager->Update();
    }

    void PlayerEntity::InitializeManager()
    {
        manager = std::make_unique<PlayerManager>(this, getPlayerInput, onSetPhotoMode, playerMaterial.get());
    }

} // namespace Beer::System
