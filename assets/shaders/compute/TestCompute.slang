#include "BeerMacros.slang"

import BeerCore;
import Noise.PerlinNoise;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _Tiling;
    float _NoiseIntensity;
    float _NoiseExponent;
    int _Seed;
    int _ThreadGroups;
};

BIND_MATERIAL_TEX(1) RWTexture2D<float4> _NoiseTex;

//$ Kernel PerlinCompute

[shader("compute")]
[numthreads(8, 8, 1)]
void PerlinCompute(uint3 id : SV_DispatchThreadID)
{
    float2 uv = float2(id.xy) / float(_ThreadGroups);
    Noise.Perlin perlin = Noise.Perlin(_Seed, _Tiling, _NoiseExponent, uv);
    _NoiseTex[id.xy] = perlin.Compute(_NoiseIntensity);
}