#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.FBM;
import Noise.PerlinNoise;
import Noise.WorleyNoise;
import Noise.CurlNoise;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float3 _Resolution;

    // red set
    int _DepthR;
    int _SeedR;
    float _FrequencyR;
    float _FrequencyMultiplierR;
    float _AmplitudeMultiplierR;
    float _NoiseBalanceR;
    float _ExponentR;
    float _EdgeMinR;
    float _EdgeMaxR;

    // green set
    int _DepthG;
    int _SeedG;
    float _FrequencyG;
    float _FrequencyMultiplierG;
    float _AmplitudeMultiplierG;
    float _NoiseBalanceG;
    float _ExponentG;
    float _EdgeMinG;
    float _EdgeMaxG;

    // blue set
    int _DepthB;
    int _SeedB;
    float _FrequencyB;
    float _FrequencyMultiplierB;
    float _AmplitudeMultiplierB;
    float _NoiseBalanceB;
    float _ExponentB;
    float _EdgeMinB;
    float _EdgeMaxB;
    
    // alpha set
    int _DepthA;
    int _SeedA;
    float _FrequencyA;
    float _FrequencyMultiplierA;
    float _AmplitudeMultiplierA;
    float _NoiseBalanceA;
    float _ExponentA;
    float _EdgeMinA;
    float _EdgeMaxA;
}

BIND_MATERIAL_TEX(1) RWTexture3D<float4> GENERATION_TARGET;
BIND_MATERIAL_TEX(2) Sampler3D<float4> _CurlSource;

//$ Kernel ComputeCloudNoise

COMP
[numthreads(8, 8, 8)]
void ComputeCloudNoise(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = (((float3)id + 0.5f) / _Resolution);

    float4 totalNoise;

    totalNoise.r = ComputePerlinWorleyFBM(uvw,
        _DepthR,
        _SeedR,
        _FrequencyR,
        _FrequencyMultiplierR,
        _AmplitudeMultiplierR,
        _ExponentR,
        _EdgeMinR,
        _EdgeMaxR,
        _NoiseBalanceR);

    totalNoise.g = ComputePerlinWorleyFBM(uvw,
        _DepthG,
        _SeedG,
        _FrequencyG,
        _FrequencyMultiplierG,
        _AmplitudeMultiplierG,
        _ExponentG,
        _EdgeMinG,
        _EdgeMaxG,
        _NoiseBalanceG);

    totalNoise.b = ComputePerlinWorleyFBM(uvw,
        _DepthB,
        _SeedB,
        _FrequencyB,
        _FrequencyMultiplierB,
        _AmplitudeMultiplierB,
        _ExponentB,
        _EdgeMinB,
        _EdgeMaxB,
        _NoiseBalanceB);

    totalNoise.a = ComputePerlinWorleyFBM(uvw,
        _DepthA,
        _SeedA,
        _FrequencyA,
        _FrequencyMultiplierA,
        _AmplitudeMultiplierA,
        _ExponentA,
        _EdgeMinA,
        _EdgeMaxA,
        _NoiseBalanceA);

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

float ComputePerlinWorleyFBM(float3 uvw,
    int depth,
    int seed,
    float frequency,
    float frequencyMul,
    float amplitudeMul,
    float exponent,
    float edgeMin,
    float edgeMax,
    float balance)
{
    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(depth,
        frequency,
        frequencyMul,
        1.0f,
        amplitudeMul);
    
    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, seed));

    Noise.FBM<WorleyComputer> worleyFBM = Noise.FBM<WorleyComputer>(depth,
        frequency,
        frequencyMul,
        1.0f,
        amplitudeMul);

    worleyFBM.SetNoiseComputer(WorleyComputer(uvw, seed));

    float perlinWorley = lerp(worleyFBM.Solve(), perlinFBM.Solve(), balance);
    perlinWorley = pow(perlinWorley, exponent);
    perlinWorley = smoothstep(edgeMin, edgeMax, perlinWorley);

    return perlinWorley;
}

//$ Kernel ComputeDistortionBase

COMP
[numthreads(8, 8, 8)]
void ComputeDistortionBase(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = (((float3)id + 0.5f) / _Resolution);
    float4 totalNoise = (float4)0;

    totalNoise.r = ComputePerlinFBM(uvw,
        _DepthR,
        _SeedR,
        _FrequencyR,
        _FrequencyMultiplierR,
        _AmplitudeMultiplierR,
        _ExponentR,
        _EdgeMinR,
        _EdgeMaxR);
    
    totalNoise.g = ComputePerlinFBM(uvw,
        _DepthR,
        _SeedR + 2345,
        _FrequencyR,
        _FrequencyMultiplierR,
        _AmplitudeMultiplierR,
        _ExponentR,
        _EdgeMinR,
        _EdgeMaxR);

    totalNoise.b = ComputePerlinFBM(uvw,
        _DepthR,
        _SeedR + 98,
        _FrequencyR,
        _FrequencyMultiplierR,
        _AmplitudeMultiplierR,
        _ExponentR,
        _EdgeMinR,
        _EdgeMaxR);

    GENERATION_TARGET[id] = totalNoise;
}

float ComputePerlinFBM(float3 uvw,
    int depth,
    int seed,
    float frequency,
    float frequencyMul,
    float amplitudeMul,
    float exponent,
    float edgeMin,
    float edgeMax)
{
    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(depth,
        frequency,
        frequencyMul,
        1.0f,
        amplitudeMul);
    
    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, seed));

    float perlin = perlinFBM.Solve();
    perlin = pow(perlin, exponent);
    perlin = smoothstep(edgeMin, edgeMax, perlin);

    return perlin;
}

public struct CurlSampler : Noise.ICurlSampler
{
    private int3 coord;

    public __init(int3 _coord)
    {
        coord = _coord;
    }

    public float3 Sample(int3 offset)
    {
        int3 res = (int3)_Resolution; 
    int3 wrappedCoord = (coord + offset + res) % res;
    
        float3 uvw = ((float3)wrappedCoord + 0.5f) / _Resolution;

        return _CurlSource.SampleLevel(uvw, 0).rgb;
    }
}

//$ Kernel ComputeDistortionCurl

COMP
[numthreads(8, 8, 8)]
void ComputeDistortionCurl(uint3 id : SV_DispatchThreadID)
{
    CurlSampler curlSampler = CurlSampler(id);  
    Noise.Curl<CurlSampler> curl = Noise.Curl<CurlSampler>(curlSampler);
    float3 rawCurl = curl.Compute();
    float3 shiftedCurl = rawCurl + 0.5f;
    float3 finalNoise = smoothstep(_EdgeMinR, _EdgeMaxR, shiftedCurl);

    GENERATION_TARGET[id] = float4(finalNoise, 0);
}
