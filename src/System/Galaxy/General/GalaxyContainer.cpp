#include "System/Galaxy/General/GalaxyContainer.hpp"
#include "Buffer/GalaxyObjectBuffer.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"
#include "System/Galaxy/General/GalaxyObjectType.hpp"

namespace Beer::System
{
    static const std::array<GalaxyBufferDefinition, 5> DEFINITIONS = {
        GalaxyBufferDefinition(GalaxyObjectType::Planet,
            "Galaxy/Planet",
            "MDL_Cube"),
        GalaxyBufferDefinition(GalaxyObjectType::Asteroids,
            "Galaxy/Planet",
            "MDL_Cube"),
        GalaxyBufferDefinition(GalaxyObjectType::SpaceGoo,
            "Galaxy/Planet",
            "MDL_Cube"),
        GalaxyBufferDefinition(GalaxyObjectType::BlackHole,
            "Galaxy/Planet",
            "MDL_Cube"),
        GalaxyBufferDefinition(GalaxyObjectType::StarDust,
            "Galaxy/Planet",
            "MDL_Cube")};

    static const uint32_t TURBULENCE_RESOLUTION = 256;
    static const VkFormat TURBULENCE_FORMAT = VK_FORMAT_R16_SFLOAT;
    static const uint32_t TURBULENCE_KERNEL = 0;
    static const uint32_t TURBULENCE_GROUPSIZE = 8;
    static const uint32_t TURBULENCE_DEPTH = 3;
    static const uint32_t TURBULENCE_SEED = 8;
    static const float TURBULENCE_FREQUENCY = 8.0f;
    static const float TURBULENCE_FREQ_MUL = 1.2f;
    static const float TURBULENCE_AMPLITUDE_MUL = 0.9f;
    static const float TURBULENCE_EXPONENT = 1.0f;

    static const uint32_t TREMOR_RESOLUTION = 256;
    static const VkFormat TREMOR_FORMAT = VK_FORMAT_R16_SFLOAT;
    static const uint32_t TREMOR_KERNEL = 0;
    static const uint32_t TREMOR_GROUPSIZE = 8;
    static const uint32_t TREMOR_DEPTH = 7;
    static const uint32_t TREMOR_SEED = 45;
    static const float TREMOR_FREQUENCY = 4.0f;
    static const float TREMOR_FREQ_MUL = 1.5f;
    static const float TREMOR_AMPLITUDE_MUL = 0.95f;
    static const float TREMOR_EXPONENT = 1.0f;

    void GalaxyContainer::CreateBuffers(const SerializableGalaxy& serializedData)
    {
        CreateNoiseVolumes();

        for (const auto& definition : DEFINITIONS)
        {
            bufferMap[definition.Type] = std::make_unique<GalaxyObjectBuffer>(serializedData,
                definition.Type,
                definition.ShaderPath,
                definition.MeshPath,
                turbulenceVolume.get(),
                tremorNoiseVolume.get());
        }
    }

    void GalaxyContainer::Draw(Rendering::CommandBuffer* commandBuffer,
        const Rendering::RenderContext& renderContext,
        const Rendering::ShaderPassType pass)
    {
        for (const auto& bufferPair : bufferMap)
        {
            bufferPair.second->Draw(commandBuffer,
                renderContext,
                pass);
        }
    }

    void GalaxyContainer::Update()
    {
        for (const auto& bufferPair : bufferMap)
        {
            bufferPair.second->Update();
        }
    }

    void GalaxyContainer::CreateNoiseVolumes()
    {
        Rendering::TextureMakeSettings makeSettings{};
        makeSettings.Width = TURBULENCE_RESOLUTION;
        makeSettings.Height = TURBULENCE_RESOLUTION;
        makeSettings.Depth = TURBULENCE_RESOLUTION;
        makeSettings.Format = TURBULENCE_FORMAT;
        makeSettings.KernelIndex = TURBULENCE_KERNEL;
        makeSettings.GroupSizeX = TURBULENCE_GROUPSIZE;
        makeSettings.GroupSizeY = TURBULENCE_GROUPSIZE;
        makeSettings.GroupSizeZ = TURBULENCE_GROUPSIZE;

        turbulenceContext = std::make_shared<Rendering::ComputeContext>("Texture/ComputeNoise3D");
        turbulenceContext->SetInt("_Depth", TURBULENCE_DEPTH);
        turbulenceContext->SetInt("_Seed", TURBULENCE_SEED);
        turbulenceContext->SetFloat("_Frequency", TURBULENCE_FREQUENCY);
        turbulenceContext->SetFloat("_AmplitudeMultiplier", TURBULENCE_AMPLITUDE_MUL);
        turbulenceContext->SetFloat("_FrequencyMultiplier", TURBULENCE_FREQ_MUL);
        turbulenceContext->SetFloat("_Exponent", TURBULENCE_EXPONENT);

        turbulenceVolume = std::make_shared<Rendering::Texture3D>(Rendering::Texture3D::Make(
            makeSettings,
            turbulenceContext.get()));

        makeSettings.Width = TREMOR_RESOLUTION;
        makeSettings.Height = TREMOR_RESOLUTION;
        makeSettings.Depth = TREMOR_RESOLUTION;
        makeSettings.Format = TREMOR_FORMAT;
        makeSettings.KernelIndex = TREMOR_KERNEL;
        makeSettings.GroupSizeX = TREMOR_GROUPSIZE;
        makeSettings.GroupSizeY = TREMOR_GROUPSIZE;
        makeSettings.GroupSizeZ = TREMOR_GROUPSIZE;

        tremorNoiseContext = std::make_shared<Rendering::ComputeContext>("Texture/ComputeNoise3D");
        tremorNoiseContext->SetInt("_Depth", TREMOR_DEPTH);
        tremorNoiseContext->SetInt("_Seed", TREMOR_SEED);
        tremorNoiseContext->SetFloat("_Frequency", TREMOR_FREQUENCY);
        tremorNoiseContext->SetFloat("_AmplitudeMultiplier", TREMOR_AMPLITUDE_MUL);
        tremorNoiseContext->SetFloat("_FrequencyMultiplier", TREMOR_FREQ_MUL);
        tremorNoiseContext->SetFloat("_Exponent", TREMOR_EXPONENT);

        tremorNoiseVolume = std::make_shared<Rendering::Texture3D>(Rendering::Texture3D::Make(
            makeSettings,
            tremorNoiseContext.get()));
    }
} // namespace Beer::System
