#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.FBM;
import Noise.PerlinNoise;
import noise.WorleyNoise;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float3 _Resolution;

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

BIND_MATERIAL_TEX(1) RWTexture3D<float4> GENERATION_TARGET;

//$ Kernel ComputePlanetNoise

COMP
[numthreads(8, 8, 8)]
void ComputePlanetNoise(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = ((float3)id / (_Resolution));

    float4 totalNoise;
    totalNoise.r = ComputeFractalPerlinNoise(uvw, _SeedR);
    totalNoise.g = ComputeFractalPerlinNoise(uvw, _SeedG);
    totalNoise.b = ComputeFractalPerlinWorleyNoise(uvw, _SeedB);
    totalNoise.a = ComputeFractalPerlinWorleyNoise(uvw, _SeedA);

    GENERATION_TARGET[id] = totalNoise;
}

public struct PerlinComputer : Noise.INoiseComputer
{
    private Noise.TileablePerlin3D perlin;

    public __init(float3 _uvw, int _seed)
    {
        perlin = Noise.TileablePerlin3D(_seed, _uvw);
    }

    public float GetNoise(float frequency)
    {
        int f = (int)frequency;
        return saturate(perlin.Compute((int3(f, f, f))) + 0.5f);
    }
}

public struct WorleyComputer : Noise.INoiseComputer
{
    private Noise.Worley3D worley;

    public __init(float3 _uvw, int _seed)
    {
        worley = Noise.Worley3D(_uvw, _seed);
    }

    public float GetNoise(float frequency)
    {
        int f = (int)frequency;
        return saturate(worley.Compute((int3(f, f, f))));
    }
}

float ComputeFractalPerlinNoise(float3 uvw, int seed)
{
    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, seed));

    float perlin = perlinFBM.Solve();
    perlin = pow(perlin, _Exponent);
    perlin = smoothstep(_EdgeMin, _EdgeMax, perlin);

    return perlin;
}

float ComputeFractalPerlinWorleyNoise(float3 uvw, int seed)
{
    Noise.FBM<WorleyComputer> worleyFBM = Noise.FBM<WorleyComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    worleyFBM.SetNoiseComputer(WorleyComputer(uvw, seed));

    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, seed));

    float perlinWorley = lerp(worleyFBM.Solve(), perlinFBM.Solve(), saturate(_NoiseBalance));
    perlinWorley = pow(perlinWorley, _Exponent);
    perlinWorley = smoothstep(_EdgeMin, _EdgeMax, perlinWorley);

    return perlinWorley;
}