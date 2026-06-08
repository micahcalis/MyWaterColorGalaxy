#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Cubemap;
import Noise.FBM;
import Noise.PerlinNoise;
import Noise.WorleyNoise;
import Blur.GaussianBlurring;

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

    int _BlurDepth;
    float _BlurRadius;
    float _BlurSpread;
}

BIND_MATERIAL_TEX(1) RWTexture2DArray<float3> GENERATION_TARGET;

//$ Kernel ComputeSkyboxNoise

COMP
[numthreads(8, 8, 1)]
void ComputeSkyboxNoise(uint3 id :SV_DispatchThreadID)
{
    float2 uv = ((float2)id.xy / _Resolution) * 2.0f - 1.0f;
    CubeFace face = (CubeFace)id.z;

    float3 cubeUvw = GetDirFromCubeFace(uv, face);

    int[3] seeds =  { _SeedR, _SeedG, _SeedB };
    float3 write;

    for (int i = 0; i < 3; i++)
    {
        write[i] = ComputeCloudNoise(cubeUvw, seeds[i]);
    }

    GENERATION_TARGET[id] = write;
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

float ComputeCloudNoise(float3 uvw, int seed)
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

BIND_MATERIAL_TEX(2) SamplerCube<float3> _BlurSource;

//$ Kernel BlurSkyboxNoise

COMP
[numthreads(8, 8, 1)]
void BlurSkyboxNoise(uint3 id : SV_DispatchThreadID)
{
    float2 uv = ((float2)id.xy / _Resolution) * 2.0f - 1.0f;
    CubeFace face = (CubeFace)id.z;

    float3 cubeUvw = normalize(GetDirFromCubeFace(uv, face));
    float sign = cubeUvw.z >= 0.0f ? 1.0f : -1.0f;
    float a = -1.0f / (sign + cubeUvw.z);
    float b = cubeUvw.x * cubeUvw.y * a;
    float3 right = float3(1.0f + sign * cubeUvw.x * cubeUvw.x * a, sign * b, -sign * cubeUvw.x);
    float3 upOrth = float3(b, sign + cubeUvw.y * cubeUvw.y * a, -cubeUvw.y);

    float2 radius = _BlurRadius * rcp(_Resolution);

    float3 colorSum = 0;
    float weightSum = 1e-5f;

    for (int y = -_BlurDepth; y <= _BlurDepth; y++)
    {
        for (int x = -_BlurDepth; x <= _BlurDepth; x++)
        {
            float2 offset = float2(x, y) * radius;
            float dist = length(offset);
            float gaussWeight = Blur.Functions.GaussWeight(length(float2(x, y)), _BlurSpread);

            float3 sampleDir = normalize(cubeUvw + (right * offset.x) + (upOrth * offset.y));
            float3 sample = _BlurSource.SampleLevel(sampleDir, 0);

            colorSum += sample * gaussWeight;
            weightSum += gaussWeight;
        }
    }

    GENERATION_TARGET[id] = colorSum / weightSum;
}