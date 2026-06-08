#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.FBM;
import Noise.VoronoiNoise;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float3 _Resolution;

    int _Depth;
    float _Frequency;
    float _FrequencyMultiplier;
    float _AmplitudeMultiplier;
    int _Seed;
}

BIND_MATERIAL_TEX(1) RWTexture3D<float> GENERATION_TARGET;

public struct VoronoiComputer : Noise.INoiseComputer
{
    private Noise.Voronoi3D voronoi;

    public __init(float3 _uvw, int _seed)
    {
        voronoi = Noise.Voronoi3D(_uvw, _seed);
    }

    public float GetNoise(float frequency)
    {
        int f = (int)frequency;
        return saturate(voronoi.Compute((int3(f, f, f))).Distance);
    }
}

//$ Kernel ComputeAsteroidNoise

COMP
[numthreads(8, 8, 8)]
void ComputeAsteroidNoise(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = ((float3)id + 0.5f) / _Resolution;

    Noise.FBM<VoronoiComputer> voronoiFBM = Noise.FBM<VoronoiComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    voronoiFBM.SetNoiseComputer(VoronoiComputer(uvw, _Seed));
    float voronoi = voronoiFBM.Solve();

    GENERATION_TARGET[id] = voronoi;
}