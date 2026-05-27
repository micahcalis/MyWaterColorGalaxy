#include "Core/BeerMacros.slang"

module GalaxyInstancing;

import Galaxy.WatercolorFunctions;
import Noise.FractalNoise;
import Core.BeerCore;
import Lights;
import Math.BeerMath;

namespace Galaxy
{
    public struct ObjectData
    {
        public float4 ColorA;
        public float4 ColorB;
        public float4 ColorC;
        public float4 ColorD;
        public float Scale;
        public uint Id;
    }
}

BIND_MATERIAL_DATA
public cbuffer InstanceGlobalData
{
    public float _Smoothness;
    public float _Wetness;
    public float _DepthBleedMin;
    public float _DepthBleedMax;
    public float _GranulationNoiseIntensity;
    public float _Hardness;
}

BIND_MATERIAL_BUFFER(1) public StructuredBuffer<float4> _DynamicGalaxyObjectPositions;
BIND_MATERIAL_BUFFER(2) public StructuredBuffer<Galaxy.ObjectData> _GalaxyObjectData;
BIND_MATERIAL_TEX(3) public Sampler3D<float4> _ControlNoiseVolume;

namespace Galaxy
{
    public float3 ToWorldSpace(float3 positionOS, uint instanceID)
    {
        float scale = _GalaxyObjectData[instanceID].Scale;
        float3 position = _DynamicGalaxyObjectPositions[instanceID].xyz;

        return positionOS * scale + position;
    }

    public float3 ToWorldSpaceBloated(float3 positionOS, uint instanceID, float bloatModifier)
    {
        float scale = _GalaxyObjectData[instanceID].Scale;
        float3 position = _DynamicGalaxyObjectPositions[instanceID].xyz;
        float bloatedScale = WC.ApplyBloat(scale, position, bloatModifier);

        return positionOS * bloatedScale + position;
    }

    public float3 GetTransformCenter(uint instanceID)
    {
        return _DynamicGalaxyObjectPositions[instanceID].xyz;
    }

    public float GetBloatedScale(float scale, float3 transformCenter, float bloatModifier)
    {
        return WC.ApplyBloat(scale, transformCenter, bloatModifier);
    }

    public ObjectData GetMaterialData(uint instanceID)
    {
        return _GalaxyObjectData[instanceID];
    }

    public struct TurbulenceNoiseSampler : Noise.INoise3DSampler
    {
        public float Sample(float3 uvw)
        {
            return _ControlNoiseVolume.Sample(uvw).r;
        }
    }

    public float GetTurbulence(float3 positionWS, float3 transformCenter)
    {
        Noise.FractalNoise<TurbulenceNoiseSampler> turbulenceNoise = Noise.FractalNoise<TurbulenceNoiseSampler>(
            TurbulenceNoiseSampler(),
            positionWS,
            transformCenter,
            1.0f);

        return turbulenceNoise.Solve();
    }

    public struct TremorNoiseSampler : Noise.INoise3DSampler
    {
        public float Sample(float3 uvw)
        {
            return _ControlNoiseVolume.Sample(uvw).g;
        }
    }

    public float2 GetTremorNoise(float3 positionWS, float3 transformCenter)
    {
        Noise.FractalNoise<TremorNoiseSampler> tremorNoise = Noise.FractalNoise<TremorNoiseSampler>(
            TremorNoiseSampler(),
            positionWS,
            transformCenter,
            1.0f);

        return float2(tremorNoise.Solve(0) - 0.5f, tremorNoise.Solve(0.5f) - 0.5f);
    }

    public float GetColorBleed(float3 positionWS, float3 normalWS)
    {
        WC.BleedComputer bleedComputer = WC.BleedComputer(positionWS,
            normalWS,
            _DepthBleedMin,
            _DepthBleedMax);

        return bleedComputer.GetBleedValue();
    }

    public struct GranulationNoiseSampler : Noise.INoise3DSampler
    {
        public float Sample(float3 uvw)
        {
            return _ControlNoiseVolume.Sample(uvw).b;      
        }
    }

    public float GetGranulation(float3 positionWS, 
        float3 transformCenter,
        float3 normalWS)
    {
        Noise.FractalNoise<GranulationNoiseSampler> granulationNoise = Noise.FractalNoise<GranulationNoiseSampler>(
            GranulationNoiseSampler(),
            positionWS,
            transformCenter,
            1.2f);

        float noise = (granulationNoise.Solve() * _GranulationNoiseIntensity) - 0.5f;
        float3 noiseNormal = normalize(normalWS + noise);
        Light light = GetMainLight(positionWS);
        float NdotL = saturate(dot(noiseNormal, light.GetDirection()) * 0.5f + 0.5f);

        return 1.0f - (NdotL * NdotL * NdotL * NdotL) * 0.5f;
    }

    public float GetDistortion()
    {
        return pow(_Wetness, 0.75f) * 0.5f + 0.5f;
    }

    public struct ScrapeNoiseSampler : Noise.INoise3DSampler
    {
        public float Sample(float3 uvw)
        {
            return _ControlNoiseVolume.Sample(uvw).a;      
        }
    }

    public float GetScrapeNoise(float3 positionWS, 
        float3 transformCenter,
        float3 normalWS,
        float3 viewDirWS)
    {
        Noise.FractalNoise<ScrapeNoiseSampler> scrapeNoise = Noise.FractalNoise<ScrapeNoiseSampler>(
            ScrapeNoiseSampler(),
            positionWS,
            transformCenter,
            0.9f);

        float noise = scrapeNoise.Solve();
        float threshold = Math.Remap(_Hardness, float2(0, 1), float2(0.6f, 0.45f));
        float scrape = step(threshold, noise);

        float mask = smoothstep(0.5f, 0.45f, (1.0f - saturate(dot(normalWS, -viewDirWS))));
        return scrape * mask;
    }
}