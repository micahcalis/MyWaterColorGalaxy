#include "System/Galaxy/Player/PlayerVFXEntity.hpp"
#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Drawing/RenderRegister.hpp"

namespace Beer::System
{
    static const float FLAME_SCALE = 2.0f;
    static const float WETNESS = 0.0f;
    static const float GRANULATION_NOISE_INTENSITY = 1.0f;
    static const glm::vec4 INNER_FLAME_COLOR = glm::vec4(1, 1, 1, 0.8f);
    static const glm::vec4 OUTER_FLAME_COLOR = glm::vec4(0.2f, 0.2f, 1, 0.5f);

    PlayerVFXEntity::PlayerVFXEntity(Function<Transform> getPlayerTransformData,
        Function<bool> isBoosting,
        std::shared_ptr<Rendering::Texture3D> controlNoiseVolume,
        Layer layer)
        : getPlayerTransformData(getPlayerTransformData)
        , isBoosting(isBoosting)
        , controlNoiseVolume(controlNoiseVolume)
        , GameEntity(layer)
    {
        transform.Scale = glm::vec3(FLAME_SCALE);

        flameMaterial = std::make_shared<Rendering::Material>("Galaxy/BoostFlame");
        flameMaterial->SetColor("_InnerFlameColor", INNER_FLAME_COLOR);
        flameMaterial->SetColor("_OuterFlameColor", OUTER_FLAME_COLOR);
        flameMaterial->SetFloat("_Wetness", WETNESS);
        flameMaterial->SetFloat("_GranulationNoiseIntensity", GRANULATION_NOISE_INTENSITY);
        flameMaterial->SetTexture("_ControlNoiseVolume", controlNoiseVolume.get());

        flameMesh = Rendering::Mesh::Get("MDL_Cube");

        std::unique_ptr<SingleMeshRender> singleMeshRender
            = RenderRegister::CreateRenderComponent<SingleMeshRender>(
                ContextType::Galaxy,
                flameMaterial,
                flameMesh,
                &this->transform,
                &this->layer);

        this->renderComponent = (std::move(singleMeshRender));
    }
} // namespace Beer::System
