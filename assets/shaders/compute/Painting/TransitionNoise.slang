#include "Core/BeerMacros.slang"

import Noise.FBM;
import Noise.PerlinNoise;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float2 _Resolution;
    int _Depth;
    int _Seed;
    float _Frequency;
    float _FrequencyMultiplier;
    float _AmplitudeMultiplier;
    float _Exponent;
    float _Edge;
}

BIND_MATERIAL_TEX(1) RWTexture2D<float> GENERATION_TARGET;

public struct PerlinComputer : Noise.INoiseComputer
{
    private Noise.TileablePerlin perlin;

    public __init(float2 _uv, int _seed)
    {
        perlin = Noise.TileablePerlin(_seed, _uv);
    }

    public float GetNoise(float frequency)
    {
        int f = (int)frequency;
        return saturate(perlin.Compute((int2(f, f))) * 0.5f + 0.5f);
    }
}

//$ Kernel ComputeTransitionNoise

COMP
[numthreads(8, 8, 1)]
void ComputeTransitionNoise(uint3 id : SV_DispatchThreadID)
{
    float2 uv = ((float2)id.xy + 0.5f) / (_Resolution);

    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    perlinFBM.SetNoiseComputer(PerlinComputer(uv, _Seed));

    float perlin = perlinFBM.Solve();
    perlin = pow(perlin, _Exponent);
    perlin = smoothstep(0, _Edge, perlin);

    GENERATION_TARGET[id.xy] = perlin;
}