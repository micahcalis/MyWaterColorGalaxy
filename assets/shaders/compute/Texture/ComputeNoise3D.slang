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

BIND_MATERIAL_TEX(1) RWTexture3D<float> GENERATION_TARGET;

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

//$ Kernel ComputePerlin

COMP
[numthreads(8, 8, 8)]
void ComputePerlin(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = ((float3)id / (_Resolution));

    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, _Seed));

    float perlin = perlinFBM.Solve();
    perlin = pow(perlin, _Exponent);
    
    GENERATION_TARGET[id] = perlin;
}

//$ Kernel ComputePerlinWorley

COMP
[numthreads(8, 8, 8)]
void ComputePerlinWorley(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = ((float3)id / (_Resolution));

    Noise.FBM<WorleyComputer> worleyFBM = Noise.FBM<WorleyComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    worleyFBM.SetNoiseComputer(WorleyComputer(uvw, _Seed));

    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);
    
    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, _Seed));

    float perlinWorley = lerp(worleyFBM.Solve(), perlinFBM.Solve(), saturate(_NoiseBalance));
    perlinWorley = pow(perlinWorley, _Exponent);
    perlinWorley = smoothstep(0, _Edge, perlinWorley);
    
    GENERATION_TARGET[id] = perlinWorley;
}