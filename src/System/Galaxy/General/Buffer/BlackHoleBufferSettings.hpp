#pragma once

#include "System/Galaxy/General/Buffer/GalaxyBufferSettings.hpp"

namespace Beer::System
{
    static const float BLHOLE_SPEED_MUL = 0.02f;
    static const float BLHOLE_BLEED_MIN = 200.0f;
    static const float BLHOLE_BLEED_MAX = 600.0f;
    static const float BLHOLE_GRANULATION_NOISE = 0.5f;
    static const float BLHOLE_HARDNESS = 0.0f;
    static const float BLHOLE_SMOOTHNESS = 0.0f;
    static const float BLHOLE_WETNESS = 0.8f;

    static const std::string BLHOLE_MESH_PATH = "MDL_Cube";

    class BlackHoleBuferSettings : public IGalaxyBufferSettings
    {
    public:
        BlackHoleBuferSettings()
        {
            SpeedMultiplier = BLHOLE_SPEED_MUL;
            DepthBleedMin = BLHOLE_BLEED_MIN;
            DepthBleedMax = BLHOLE_BLEED_MAX;
            GranulationNoiseIntensity = BLHOLE_GRANULATION_NOISE;
            Hardness = BLHOLE_HARDNESS;
            Smoothness = BLHOLE_SMOOTHNESS;
            Wetness = BLHOLE_WETNESS;
        }

        std::shared_ptr<Rendering::Mesh> GetMesh() override
        {
            return Rendering::Mesh::Get(BLHOLE_MESH_PATH);
        }

        void ApplyMaterialSettings(Rendering::Material* material) override
        {
            IGalaxyBufferSettings::ApplyMaterialSettings(material);
        }
    };
} // namespace Beer::System
