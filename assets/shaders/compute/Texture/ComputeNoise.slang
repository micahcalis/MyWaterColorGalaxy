#include "Core/BeerMacros.slang"

import Noise.FBM;
import Noise.WorleyNoise;
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
    float _NoiseBalance;
    float _Exponent;
    float _Edge;
}

BIND_MATERIAL_TEX(1) RWTexture2D<float4> GENERATION_TARGET;

//$ Kernel ComputePerlinWorley

public struct WorleyComputer : Noise.INoiseComputer
{
    private Noise.Worley worley;

    public __init(float2 _uv, int _seed)
    {
        worley = Noise.Worley(_uv, _seed);
    }

    public float GetNoise(float frequency)
    {
        return worley.Compute((int)frequency);
    }
}

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

COMP
[numthreads(8, 8, 1)]
void ComputePerlinWorley(uint3 id : SV_DispatchThreadID)
{
    float2 uv = ((float2)id.xy) / (_Resolution);

    Noise.FBM<WorleyComputer> worleyFBM = Noise.FBM<WorleyComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    worleyFBM.SetNoiseComputer(WorleyComputer(uv, _Seed));

    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    perlinFBM.SetNoiseComputer(PerlinComputer(uv, _Seed));

    float perlinWorley = lerp(worleyFBM.Solve(), perlinFBM.Solve(), saturate(_NoiseBalance));
    perlinWorley = pow(perlinWorley, _Exponent);
    perlinWorley = smoothstep(0, _Edge, perlinWorley);
    
    GENERATION_TARGET[id.xy] = float4(saturate(perlinWorley), 0, 0, 1);
}