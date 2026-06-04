#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"
#include "System/Galaxy/General/Buffer/GalaxyBufferSettings.hpp"
#include "System/PaintTool/PaintToolContext.hpp"
#include <print>

namespace Beer::System
{
    static const float PLNT_SPEED_MUL = 0.02f;
    static const float PLNT_BLEED_MIN = 200.0f;
    static const float PLNT_BLEED_MAX = 600.0f;
    static const float PLNT_GRANULATION_NOISE = 0.4f;
    static const float PLNT_HARDNESS = 0.8f;
    static const float PLNT_SMOOTHNESS = 0.0f;
    static const float PLNT_WETNESS = 0.8f;

    static const uint32_t PLNT_NOISE_RES = 256;
    static const VkFormat PLNT_NOISE_FORMAT = VK_FORMAT_R8G8B8A8_SNORM;
    static const uint32_t PLNT_NOISE_GROUPSIZE = 8;
    static const uint32_t PLNT_NOISE_KERNEL = 0;
    static const int32_t PLNT_NOISE_DEPTH = 4;
    static const float PLNT_NOISE_FRQ = 4.0f;
    static const float PLNT_NOISE_FRQ_MUL = 1.5f;
    static const float PLNT_NOISE_AMP_MUL = 0.85f;
    static const float PLNT_NOISE_BALANCE = 0.75f;
    static const float PLNT_NOISE_EXP = 2.0f;
    static const int32_t PLNT_NOISE_SEEDR = 345;
    static const int32_t PLNT_NOISE_SEEDG = 7433;
    static const int32_t PLNT_NOISE_SEEDB = 77;
    static const int32_t PLNT_NOISE_SEEDA = 2342;
    static const float PLNT_NOISE_EDGE_MIN = 0.02f;
    static const float PLNT_NOISE_EDGE_MAX = 0.7f;

    static const std::string PLANET_MESH_PATH = "MDL_Cube";

    class PlanetBufferSettings : public IGalaxyBufferSettings
    {
    private:
        std::shared_ptr<Rendering::ComputeContext> planetNoiseContext = nullptr;

    public:
        std::shared_ptr<Rendering::Texture3D> planetNoiseVolume = nullptr;

    public:
        PlanetBufferSettings()
        {
            SpeedMultiplier = PLNT_SPEED_MUL;
            DepthBleedMin = PLNT_BLEED_MIN;
            DepthBleedMax = PLNT_BLEED_MAX;
            GranulationNoiseIntensity = PLNT_GRANULATION_NOISE;
            Hardness = PLNT_HARDNESS;
            Smoothness = PLNT_SMOOTHNESS;
            Wetness = PLNT_WETNESS;
        }

        std::shared_ptr<Rendering::Mesh> GetMesh() override
        {
            return Rendering::Mesh::Get(PLANET_MESH_PATH);
        }

        void ApplyMaterialSettings(Rendering::Material* material) override
        {
            Rendering::TextureMakeSettings makeSettings{};
            makeSettings.Width = PLNT_NOISE_RES;
            makeSettings.Height = PLNT_NOISE_RES;
            makeSettings.Depth = PLNT_NOISE_RES;
            makeSettings.GroupSizeX = PLNT_NOISE_GROUPSIZE;
            makeSettings.GroupSizeY = PLNT_NOISE_GROUPSIZE;
            makeSettings.GroupSizeZ = PLNT_NOISE_GROUPSIZE;
            makeSettings.Format = PLNT_NOISE_FORMAT;
            makeSettings.KernelIndex = PLNT_NOISE_KERNEL;

            planetNoiseContext = std::make_shared<Rendering::ComputeContext>("Watercolor/PlanetNoise");
            planetNoiseContext->SetInt("_Depth", PLNT_NOISE_DEPTH);
            planetNoiseContext->SetFloat("_Frequency", PLNT_NOISE_FRQ);
            planetNoiseContext->SetFloat("_FrequencyMultiplier", PLNT_NOISE_FRQ_MUL);
            planetNoiseContext->SetFloat("_AmplitudeMultiplier", PLNT_NOISE_AMP_MUL);
            planetNoiseContext->SetFloat("_NoiseBalance", PLNT_NOISE_BALANCE);
            planetNoiseContext->SetFloat("_Exponent", PLNT_NOISE_EXP);
            planetNoiseContext->SetFloat("_EdgeMin", PLNT_NOISE_EDGE_MIN);
            planetNoiseContext->SetFloat("_EdgeMax", PLNT_NOISE_EDGE_MAX);
            planetNoiseContext->SetInt("_SeedR", PLNT_NOISE_SEEDR);
            planetNoiseContext->SetInt("_SeedG", PLNT_NOISE_SEEDG);
            planetNoiseContext->SetInt("_SeedB", PLNT_NOISE_SEEDB);
            planetNoiseContext->SetInt("_SeedA", PLNT_NOISE_SEEDA);

            planetNoiseVolume = std::make_shared<Rendering::Texture3D>(Rendering::Texture3D::Make(
                makeSettings,
                planetNoiseContext.get()));

            IGalaxyBufferSettings::ApplyMaterialSettings(material);
            material->SetTexture("_PlanetNoiseVolume", planetNoiseVolume.get());
        }
    };
} // namespace Beer::System
