#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Cubemap;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float2 _Resolution;

    int _Depth;
    float _Frequency;
    float _FrequencyMultiplier;
    float _AmplitudeMultiplier;
    float _NoiseBalance;
    float _Exponent;
    float _EdgeMin;
    float _EdgeMax;

    int _SeedR;
    int _SeedG;
    int _SeedB;
    int _SeedA;
}

BIND_MATERIAL_TEX(1) RWTexture2DArray<float4> GENERATION_TARGET;

//$ Kernel ComputeSkyboxNoise

COMP
[numthreads(8, 8, 1)]
void ComputeSkyboxNoise(uint3 id :SV_DispatchThreadID)
{
    float2 uv = ((float2)id.xy / _Resolution) * 2.0f - 1.0f;
    CubeFace face = (CubeFace)id.z;

    float3 cubeDirection = GetDirFromCubeFace(uv, face);

    GENERATION_TARGET[id] = float4(abs(cubeDirection), 1);
}