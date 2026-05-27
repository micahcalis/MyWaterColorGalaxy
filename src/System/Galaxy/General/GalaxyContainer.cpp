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

    static const uint32_t CONTROL_RESOLUTION = 256;
    static const VkFormat CONTROL_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
    static const uint32_t CONTROL_KERNEL = 0;
    static const uint32_t CONTROL_GROUPSIZE = 8;

    static const uint32_t TURBULENCE_DEPTH = 3;
    static const uint32_t TURBULENCE_SEED = 8;
    static const float TURBULENCE_FREQUENCY = 8.0f;
    static const float TURBULENCE_FREQ_MUL = 1.2f;
    static const float TURBULENCE_AMPLITUDE_MUL = 0.9f;
    static const float TURBULENCE_EXPONENT = 1.0f;

    static const uint32_t TREMOR_DEPTH = 7;
    static const uint32_t TREMOR_SEED = 45;
    static const float TREMOR_FREQUENCY = 4.0f;
    static const float TREMOR_FREQ_MUL = 1.5f;
    static const float TREMOR_AMPLITUDE_MUL = 0.95f;
    static const float TREMOR_EXPONENT = 1.0f;

    static const uint32_t GRANULATION_DEPTH = 1;
    static const uint32_t GRANULATION_SEED = 87;
    static const float GRANULATION_FREQUENCY = 4.0f;
    static const float GRANULATION_FREQ_MUL = 1.5f;
    static const float GRANULATION_AMPLITUDE_MUL = 0.95f;
    static const float GRANULATION_EXPONENT = 1.0f;
    static const float GRANULATION_NOISEBALANCE = 0.7f;

    static const uint32_t SCRAPE_DEPTH = 3;
    static const uint32_t SCRAPE_SEED = 15;
    static const float SCRAPE_FREQUENCY = 8.0f;
    static const float SCRAPE_FREQ_MUL = 1.5f;
    static const float SCRAPE_AMPLITUDE_MUL = 1.0f;
    static const float SCRAPE_EXPONENT = 0.5f;

    void GalaxyContainer::CreateBuffers(const SerializableGalaxy& serializedData)
    {
        CreateNoiseVolumes();

        for (const auto& definition : DEFINITIONS)
        {
            bufferMap[definition.Type] = std::make_unique<GalaxyObjectBuffer>(serializedData,
                definition.Type,
                definition.ShaderPath,
                definition.MeshPath,
                controlNoiseVolume.get());
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
        makeSettings.Width = CONTROL_RESOLUTION;
        makeSettings.Height = CONTROL_RESOLUTION;
        makeSettings.Depth = CONTROL_RESOLUTION;
        makeSettings.Format = CONTROL_FORMAT;
        makeSettings.KernelIndex = CONTROL_KERNEL;
        makeSettings.GroupSizeX = CONTROL_GROUPSIZE;
        makeSettings.GroupSizeY = CONTROL_GROUPSIZE;
        makeSettings.GroupSizeZ = CONTROL_GROUPSIZE;

        controlNoiseContext = std::make_shared<Rendering::ComputeContext>("Watercolor/ControlNoise");

        controlNoiseContext->SetInt("_DepthR", TURBULENCE_DEPTH);
        controlNoiseContext->SetInt("_SeedR", TURBULENCE_SEED);
        controlNoiseContext->SetFloat("_FrequencyR", TURBULENCE_FREQUENCY);
        controlNoiseContext->SetFloat("_AmplitudeMultiplierR", TURBULENCE_AMPLITUDE_MUL);
        controlNoiseContext->SetFloat("_FrequencyMultiplierR", TURBULENCE_FREQ_MUL);
        controlNoiseContext->SetFloat("_ExponentR", TURBULENCE_EXPONENT);

        controlNoiseContext->SetInt("_DepthG", TREMOR_DEPTH);
        controlNoiseContext->SetInt("_SeedG", TREMOR_SEED);
        controlNoiseContext->SetFloat("_FrequencyG", TREMOR_FREQUENCY);
        controlNoiseContext->SetFloat("_AmplitudeMultiplierG", TREMOR_AMPLITUDE_MUL);
        controlNoiseContext->SetFloat("_FrequencyMultiplierG", TREMOR_FREQ_MUL);
        controlNoiseContext->SetFloat("_ExponentG", TREMOR_EXPONENT);

        controlNoiseContext->SetInt("_DepthB", GRANULATION_DEPTH);
        controlNoiseContext->SetInt("_SeedB", GRANULATION_SEED);
        controlNoiseContext->SetFloat("_FrequencyB", GRANULATION_FREQUENCY);
        controlNoiseContext->SetFloat("_AmplitudeMultiplierB", GRANULATION_AMPLITUDE_MUL);
        controlNoiseContext->SetFloat("_FrequencyMultiplierB", GRANULATION_FREQ_MUL);
        controlNoiseContext->SetFloat("_ExponentB", GRANULATION_EXPONENT);
        controlNoiseContext->SetFloat("_NoiseBalanceB", GRANULATION_NOISEBALANCE);

        controlNoiseContext->SetInt("_DepthA", SCRAPE_DEPTH);
        controlNoiseContext->SetInt("_SeedA", SCRAPE_SEED);
        controlNoiseContext->SetFloat("_FrequencyA", SCRAPE_FREQUENCY);
        controlNoiseContext->SetFloat("_AmplitudeMultiplierA", SCRAPE_AMPLITUDE_MUL);
        controlNoiseContext->SetFloat("_FrequencyMultiplierA", SCRAPE_FREQ_MUL);
        controlNoiseContext->SetFloat("_ExponentA", SCRAPE_EXPONENT);

        controlNoiseVolume = std::make_shared<Rendering::Texture3D>(Rendering::Texture3D::Make(makeSettings,
            controlNoiseContext.get()));
    }
} // namespace Beer::System
