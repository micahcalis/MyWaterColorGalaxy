#pragma once

#include "System/Galaxy/General/Buffer/GalaxyBufferSettings.hpp"

namespace Beer::System
{
    static const float GOO_SPEED_MUL = 0.02f;
    static const float GOO_BLEED_MIN = 200.0f;
    static const float GOO_BLEED_MAX = 600.0f;
    static const float GOO_GRANULATION_NOISE = 0.6f;
    static const float GOO_HARDNESS = 0.0f;
    static const float GOO_SMOOTHNESS = 0.5f;
    static const float GOO_WETNESS = 0.8f;

    static const std::string GOO_MESH_PATH = "MDL_Cube";

    class SpaceGooBufferSettings : public IGalaxyBufferSettings
    {
    public:
        SpaceGooBufferSettings()
        {
            SpeedMultiplier = GOO_SPEED_MUL;
            DepthBleedMin = GOO_BLEED_MIN;
            DepthBleedMax = GOO_BLEED_MAX;
            GranulationNoiseIntensity = GOO_GRANULATION_NOISE;
            Hardness = GOO_HARDNESS;
            Smoothness = GOO_SMOOTHNESS;
            Wetness = GOO_WETNESS;
        }

        std::shared_ptr<Rendering::Mesh> GetMesh() override
        {
            return Rendering::Mesh::Get(GOO_MESH_PATH);
        }

        void ApplyMaterialSettings(Rendering::Material* material) override
        {
            IGalaxyBufferSettings::ApplyMaterialSettings(material);
        }
    };
} // namespace Beer::System
