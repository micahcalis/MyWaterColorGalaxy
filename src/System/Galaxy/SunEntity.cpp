#include "System/Galaxy/General/SunEntity.hpp"
#include "General/Buffer/GalaxyObjectBuffer.hpp"
#include "System/Components/General/SingleMeshRender.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Light/ILight.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapEntity.hpp"

namespace Beer::System
{
    static const float SMOOTHNESS = 0.5f;
    static const float HARDNESS = 0.0f;
    static const float WETNESS = 0.8f;
    static const float GRANULATION_NOISE_INTENSITY = 1.0f;
    static const float DEPTH_BLEED_MIN = 200.0f;
    static const float DEPTH_BLEED_MAX = 600.0f;

    void SunEntity::LoadFromSerialized(const SerializableGalaxy& serializedGalaxy)
    {
        sunMaterial = std::make_shared<Rendering::Material>("Galaxy/Sun");
        sunMaterial->SetColor("_SunColor", serializedGalaxy.StarColor);
        sunMaterial->SetTexture("_ControlNoiseVolume", controlNoiseVolume.get());
        sunMaterial->SetFloat("_Smoothness", SMOOTHNESS);
        sunMaterial->SetFloat("_Hardness", HARDNESS);
        sunMaterial->SetFloat("_Wetness", WETNESS);
        sunMaterial->SetFloat("_GranulationNoiseIntensity", GRANULATION_NOISE_INTENSITY);
        sunMaterial->SetFloat("_DepthBleedMin", DEPTH_BLEED_MIN);
        sunMaterial->SetFloat("_DepthBleedMax", DEPTH_BLEED_MAX);

        sunMesh = Rendering::Mesh::Get("MDL_Cube");

        transform.Position = glm::vec3(serializedGalaxy.StarPosition.x, 0, serializedGalaxy.StarPosition.y) * GalaxyObjectBuffer::GALAXY_POS_SCALE;
        transform.Scale = glm::vec3(serializedGalaxy.StarSize
            * GalaxyObjectBuffer::GALAXY_SIZE_SCALE
            * GalaxyMapEntity::CENTER_STAR_SCALE
            * 1.3f);

        pointLight = ILight::CreateLight<PointLight>(&transform,
            15,
            serializedGalaxy.StarColor,
            glm::vec4(0.2, 0.23, 0.35, 1),
            glm::vec4(0.86, 0.98, 1, 1));

        renderComponent = RenderRegister::CreateRenderComponent<SingleMeshRender>(ContextType::Galaxy,
            sunMaterial,
            sunMesh,
            &transform,
            &layer);
    }
} // namespace Beer::System
