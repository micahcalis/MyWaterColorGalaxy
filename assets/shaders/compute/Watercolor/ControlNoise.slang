#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.FBM;
import Noise.PerlinNoise;
import Noise.WorleyNoise;
import Noise.VoronoiNoise;

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
    float _EdgeR;

    // green set
    int _DepthG;
    int _SeedG;
    float _FrequencyG;
    float _FrequencyMultiplierG;
    float _AmplitudeMultiplierG;
    float _NoiseBalanceG;
    float _ExponentG;
    float _EdgeG;

    // blue set
    int _DepthB;
    int _SeedB;
    float _FrequencyB;
    float _FrequencyMultiplierB;
    float _AmplitudeMultiplierB;
    float _NoiseBalanceB;
    float _ExponentB;
    float _EdgeB;
    
    // alpha set
    int _DepthA;
    int _SeedA;
    float _FrequencyA;
    float _FrequencyMultiplierA;
    float _AmplitudeMultiplierA;
    float _NoiseBalanceA;
    float _ExponentA;
    float _EdgeA;
}

BIND_MATERIAL_TEX(1) RWTexture3D<float4> GENERATION_TARGET;

//$ Kernel ComputeControlNoise

COMP
[numthreads(8, 8, 8)]
void ComputeControlNoise(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = ((float3)id / (_Resolution));

    float4 totalNoise;
    totalNoise.r = ComputeTurbulence(uvw);
    totalNoise.g = ComputeTremor(uvw);
    totalNoise.b = ComputeGranulation(uvw);
    totalNoise.a = ComputeScrape(uvw);

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

public struct VoronoiPolygonalComputer : Noise.INoiseComputer
{
    private Noise.VoronoiPolygonal3D voronoi;

    public __init(float3 _uvw, int _seed)
    {
        voronoi = Noise.VoronoiPolygonal3D(_uvw, _seed);
    }

    public float GetNoise(float frequency)
    {
        return saturate(voronoi.Compute(float3(frequency)).Distance);
    }
}

float ComputeTurbulence(float3 uvw)
{
    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_DepthR,
        _FrequencyR,
        _FrequencyMultiplierR,
        1.0f,
        _AmplitudeMultiplierR);

    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, _SeedR));

    float perlin = perlinFBM.Solve();
    perlin = pow(perlin, _ExponentR);

    return perlin;
}

float ComputeTremor(float3 uvw)
{
    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_DepthG,
        _FrequencyG,
        _FrequencyMultiplierG,
        1.0f,
        _AmplitudeMultiplierG);

    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, _SeedG));

    float perlin = perlinFBM.Solve();
    perlin = pow(perlin, _ExponentG);
    
    return perlin;
}

float ComputeGranulation(float3 uvw)
{
    Noise.FBM<WorleyComputer> worleyFBM = Noise.FBM<WorleyComputer>(_DepthB,
        _FrequencyB,
        _FrequencyMultiplierB,
        1.0f,
        _AmplitudeMultiplierB);

    worleyFBM.SetNoiseComputer(WorleyComputer(uvw, _SeedB));

    Noise.FBM<PerlinComputer> perlinFBM = Noise.FBM<PerlinComputer>(_DepthB,
        _FrequencyB,
        _FrequencyMultiplierB,
        1.0f,
        _AmplitudeMultiplierB);

    perlinFBM.SetNoiseComputer(PerlinComputer(uvw, _SeedB));

    float perlinWorley = lerp(worleyFBM.Solve(), perlinFBM.Solve(), saturate(_NoiseBalanceB));
    perlinWorley = pow(perlinWorley, _ExponentB);

    return perlinWorley;
}

float ComputeScrape(float3 uvw)
{
    Noise.FBM<VoronoiPolygonalComputer> voronoiFBM = Noise.FBM<VoronoiPolygonalComputer>(_DepthA,
         _FrequencyA,
        _FrequencyMultiplierA,
        1.0f,
        _AmplitudeMultiplierA);

    voronoiFBM.SetNoiseComputer(VoronoiPolygonalComputer(uvw, _SeedA));

    float voronoi = voronoiFBM.Solve();
    voronoi = pow(voronoi, _ExponentA);

    return voronoi;
}