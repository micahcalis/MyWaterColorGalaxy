#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.FBM;
import Noise.WorleyNoise;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float3 _Resolution;

    int _Depth;
    float _Frequency;
    float _FrequencyMultiplier;
    float _AmplitudeMultiplier;
    float _Exponent;

    int _SeedR;
    int _SeedG;
}

BIND_MATERIAL_TEX(1) RWTexture3D<float2> GENERATION_TARGET;

//$ Kernel ComputeSunNoise

COMP
[numthreads(8, 8, 8)]
void ComputeSunNoise(uint3 id : SV_DispatchThreadID)
{
    float3 uvw = ((float3)id + 0.5f) / _Resolution;

    float2 totalNoise;
    totalNoise.r = ComputeFractalWorleyNoise(uvw, _SeedR);
    totalNoise.g = ComputeFractalWorleyNoise(uvw, _SeedG);

    GENERATION_TARGET[id] = totalNoise;
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

float ComputeFractalWorleyNoise(float3 uvw, int seed)
{
    Noise.FBM<WorleyComputer> worleyFBM = Noise.FBM<WorleyComputer>(_Depth,
        _Frequency,
        _FrequencyMultiplier,
        1.0f,
        _AmplitudeMultiplier);

    worleyFBM.SetNoiseComputer(WorleyComputer(uvw, seed));

    float worley = worleyFBM.Solve();
    worley = pow(worley,  _Exponent);
    
    return worley;
}