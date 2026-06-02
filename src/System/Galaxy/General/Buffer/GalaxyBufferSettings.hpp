#pragma once

#include "Rendering/Material/Material.hpp"

namespace Beer::System
{
    class IGalaxyBufferSettings
    {
    public:
        float SpeedMultiplier = 0.02f;
        float DepthBleedMin = 200.0f;
        float DepthBleedMax = 600.0f;
        float GranulationNoiseIntensity = 0.2f;
        float Hardness = 0.2f;
        float Smoothness = 0.5f;
        float Wetness = 0.8f;

    public:
        virtual ~IGalaxyBufferSettings() = default;
        virtual void ApplyMaterialSettings(Rendering::Material* material)
        {
            material->SetFloat("_DepthBleedMin", DepthBleedMin);
            material->SetFloat("_DepthBleedMax", DepthBleedMax);
            material->SetFloat("_GranulationNoiseIntensity", GranulationNoiseIntensity);
            material->SetFloat("_Hardness", Hardness);
            material->SetFloat("_Smoothness", Smoothness);
            material->SetFloat("_Wetness", Wetness);
        }
    };

    class UnimplementedBufferSettings : public IGalaxyBufferSettings
    {
    public:
        UnimplementedBufferSettings(float speedMultiplier,
            float depthBleedMin,
            float depthBleedMax,
            float granulationNoiseIntensity,
            float hardness,
            float smoothness,
            float wetness)
        {
            SpeedMultiplier = speedMultiplier;
            DepthBleedMin = depthBleedMin;
            DepthBleedMax = depthBleedMax;
            GranulationNoiseIntensity = granulationNoiseIntensity;
            Hardness = hardness;
            Smoothness = smoothness;
            Wetness = wetness;
        }

        void ApplyMaterialSettings(Rendering::Material* material) override
        {
            IGalaxyBufferSettings::ApplyMaterialSettings(material);
        }
    };

} // namespace Beer::System
