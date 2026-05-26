#include "Core/BeerMacros.slang"

module GalaxyInstancing;

import Galaxy.WatercolorFunctions;
import Noise.FractalNoise;

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
    public float _DepthBleedMin;
    public float _DepthBleedMax;
}

BIND_MATERIAL_BUFFER(1) public StructuredBuffer<float4> _DynamicGalaxyObjectPositions;
BIND_MATERIAL_BUFFER(2) public StructuredBuffer<Galaxy.ObjectData> _GalaxyObjectData;
BIND_MATERIAL_TEX(3) public Sampler3D<float> _TurbulenceVolume;
BIND_MATERIAL_TEX(4) public Sampler3D<float> _TremorNoiseVolume;

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
            return _TurbulenceVolume.Sample(uvw);
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
            return _TremorNoiseVolume.Sample(uvw);
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
}