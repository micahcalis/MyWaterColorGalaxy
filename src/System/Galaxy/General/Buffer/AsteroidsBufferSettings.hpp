#pragma once

#include "Core/Assets/MeshAsset.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"
#include "System/Galaxy/General/Buffer/GalaxyBufferSettings.hpp"
#include <cstdint>

namespace Beer::System
{
    static const float ASTRDS_SPEED_MUL = 0.02f;
    static const float ASTRDS_BLEED_MIN = 200.0f;
    static const float ASTRDS_BLEED_MAX = 600.0f;
    static const float ASTRDS_GRANULATION_NOISE = 0.5f;
    static const float ASTRDS_HARDNESS = 1.0f;
    static const float ASTRDS_SMOOTHNESS = 0.0f;
    static const float ASTRDS_WETNESS = 0.8f;

    static const uint32_t NUM_ASTEROIDS = 30;

    static const glm::vec3 localPositions[8] = {
        {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}};

    static const uint32_t localIndices[36] = {
        0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3};

    static const uint32_t ASTRDS_NOISE_RES = 256;
    static const VkFormat ASTRDS_NOISE_FORMAT = VK_FORMAT_R16_SFLOAT;
    static const uint32_t ASTRDS_NOISE_GROUPSIZE = 8;
    static const uint32_t ASTRDS_NOISE_KERNEL = 0;
    static const int32_t ASTRDS_NOISE_DEPTH = 2;
    static const float ASTRDS_NOISE_FRQ = 8.0f;
    static const float ASTRDS_NOISE_FRQ_MUL = 1.5f;
    static const float ASTRDS_NOISE_AMPL_MUL = 0.8f;
    static const int ASTRDS_NOISE_SEED = 543;

    class AsteroidsBufferSettings : public IGalaxyBufferSettings
    {
    private:
        std::shared_ptr<Rendering::ComputeContext> asteroidNoiseContext = nullptr;

    public:
        std::shared_ptr<Rendering::Texture3D> asteroidNoiseVolume = nullptr;

    public:
        AsteroidsBufferSettings()
        {
            SpeedMultiplier = ASTRDS_SPEED_MUL;
            DepthBleedMin = ASTRDS_BLEED_MIN;
            DepthBleedMax = ASTRDS_BLEED_MAX;
            GranulationNoiseIntensity = ASTRDS_GRANULATION_NOISE;
            Hardness = ASTRDS_HARDNESS;
            Smoothness = ASTRDS_SMOOTHNESS;
            Wetness = ASTRDS_WETNESS;
        }

        std::shared_ptr<Rendering::Mesh> GetMesh() override
        {
            Core::MeshAsset proceduralAsset;

            proceduralAsset.Positions.reserve(NUM_ASTEROIDS * 8);
            proceduralAsset.VertexColors.reserve(NUM_ASTEROIDS * 8);
            proceduralAsset.Indices.reserve(NUM_ASTEROIDS * 36);

            for (uint32_t i = 0; i < NUM_ASTEROIDS; i++)
            {
                uint32_t startVertex = static_cast<uint32_t>(proceduralAsset.Positions.size());

                for (int v = 0; v < 8; v++)
                {
                    proceduralAsset.Positions.push_back(localPositions[v]);

                    proceduralAsset.VertexColors.push_back(glm::vec4(static_cast<float>(i), 0.0f, 0.0f, 1.0f));
                }

                for (int idx = 0; idx < 36; idx++)
                {
                    proceduralAsset.Indices.push_back(startVertex + localIndices[idx]);
                }
            }

            return Rendering::Mesh::Create(proceduralAsset);
        }

        void ApplyMaterialSettings(Rendering::Material* material) override
        {
            IGalaxyBufferSettings::ApplyMaterialSettings(material);

            Rendering::TextureMakeSettings makeSettings{};
            makeSettings.Width = ASTRDS_NOISE_RES;
            makeSettings.Height = ASTRDS_NOISE_RES;
            makeSettings.Depth = ASTRDS_NOISE_RES;
            makeSettings.GroupSizeX = ASTRDS_NOISE_GROUPSIZE;
            makeSettings.GroupSizeY = ASTRDS_NOISE_GROUPSIZE;
            makeSettings.GroupSizeZ = ASTRDS_NOISE_GROUPSIZE;
            makeSettings.Format = ASTRDS_NOISE_FORMAT;
            makeSettings.KernelIndex = ASTRDS_NOISE_KERNEL;

            asteroidNoiseContext = std::make_shared<Rendering::ComputeContext>("Watercolor/AsteroidNoise");
            asteroidNoiseContext->SetInt("_Depth", ASTRDS_NOISE_DEPTH);
            asteroidNoiseContext->SetFloat("_Frequency", ASTRDS_NOISE_FRQ);
            asteroidNoiseContext->SetFloat("_FrequencyMultiplier", ASTRDS_NOISE_FRQ_MUL);
            asteroidNoiseContext->SetFloat("_AmplitudeMultiplier", ASTRDS_NOISE_AMPL_MUL);
            asteroidNoiseContext->SetInt("_Seed", ASTRDS_NOISE_SEED);

            asteroidNoiseVolume = std::make_shared<Rendering::Texture3D>(Rendering::Texture3D::Make(
                makeSettings,
                asteroidNoiseContext.get()));

            material->SetTexture("_AsteroidNoiseVolume", asteroidNoiseVolume.get());
        }
    };
} // namespace Beer::System
