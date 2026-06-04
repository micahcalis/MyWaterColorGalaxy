#include "System/Galaxy/General/SunEntity.hpp"
#include "General/Buffer/GalaxyObjectBuffer.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"
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
    static const float GRADIENT_INTENSITY = 2.0f;

    static const uint32_t SUN_NOISE_RES = 256;
    static const VkFormat SUN_NOISE_FORMAT = VK_FORMAT_R8G8_UNORM;
    static const uint32_t SUN_NOISE_GROUP_SIZE = 8;
    static const uint32_t SUN_NOISE_KERNEL = 0;

    static const uint32_t SUN_NOISE_DEPTH = 2;
    static const float SUN_NOISE_FREQ = 4.0f;
    static const float SUN_NOISE_FREQ_MUL = 1.5f;
    static const float SUN_NOISE_AMP_MUL = 0.8f;
    static const float SUN_NOISE_EXP = 1.0f;
    static const uint32_t SUN_NOISE_SEEDR = 4675;
    static const uint32_t SUN_NOISE_SEEDG = 9742;

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
        sunMaterial->SetFloat("_GradientIntensity", GRADIENT_INTENSITY);

        sunMesh = Rendering::Mesh::Get("MDL_Cube");

        transform.Position = glm::vec3(serializedGalaxy.StarPosition.x, 0, serializedGalaxy.StarPosition.y) * GalaxyObjectBuffer::GALAXY_POS_SCALE;
        transform.Scale = glm::vec3(serializedGalaxy.StarSize
            * GalaxyObjectBuffer::GALAXY_SIZE_SCALE
            * GalaxyMapEntity::CENTER_STAR_SCALE
            * 3.0f);

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

        InitializeSunNoise();
    }

    void SunEntity::InitializeSunNoise()
    {
        Rendering::TextureMakeSettings makeSettings;
        makeSettings.Width = SUN_NOISE_RES;
        makeSettings.Height = SUN_NOISE_RES;
        makeSettings.Depth = SUN_NOISE_RES;
        makeSettings.Format = SUN_NOISE_FORMAT;
        makeSettings.GroupSizeX = SUN_NOISE_GROUP_SIZE;
        makeSettings.GroupSizeY = SUN_NOISE_GROUP_SIZE;
        makeSettings.GroupSizeZ = SUN_NOISE_GROUP_SIZE;
        makeSettings.KernelIndex = SUN_NOISE_KERNEL;

        sunNoiseContext = std::make_shared<Rendering::ComputeContext>("Watercolor/SunNoise");
        sunNoiseContext->SetInt("_Depth", SUN_NOISE_DEPTH);
        sunNoiseContext->SetFloat("_Frequency", SUN_NOISE_FREQ);
        sunNoiseContext->SetFloat("_FrequencyMultiplier", SUN_NOISE_FREQ_MUL);
        sunNoiseContext->SetFloat("_AmplitudeMultiplier", SUN_NOISE_AMP_MUL);
        sunNoiseContext->SetFloat("_Exponent", SUN_NOISE_EXP);
        sunNoiseContext->SetInt("_SeedR", SUN_NOISE_SEEDR);
        sunNoiseContext->SetInt("_SeedG", SUN_NOISE_SEEDG);

        sunNoiseVolume = std::make_shared<Rendering::Texture3D>(Rendering::Texture3D::Make(makeSettings,
            sunNoiseContext.get()));

        sunMaterial->SetTexture("_SunNoiseVolume", sunNoiseVolume.get());
    }
} // namespace Beer::System
