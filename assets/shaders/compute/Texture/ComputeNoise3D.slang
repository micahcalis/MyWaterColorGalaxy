#include "Core/BeerMacros.slang"

import Noise.FBM;
import Noise.WorleyNoise;
import Noise.PerlinNoise;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float3 _Resolution;
    int _Depth;
    int _Seed;
    float _Frequency;
    float _FrequencyMultiplier;
    float _AmplitudeMultiplier;
    float _NoiseBalance;
    float _Exponent;
    float _Edge;
}

BIND_MATERIAL_TEX(1) RWTexture3D<float4> GENERATION_TARGET;

//$ Kernel ComputeTest

COMP
[numthreads(8, 8, 8)]
void ComputeTest(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = ((float3)id / (_Resolution));
    GENERATION_TARGET[id] = float4(uvw, 1);
}