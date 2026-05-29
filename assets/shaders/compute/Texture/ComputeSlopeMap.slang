#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Math.PartialDerivative;

BIND_MATERIAL_DATA
cbuffer ComputeSettings
{
    float2 _Resolution;
    float2 _SourceTexelSize;
}

BIND_MATERIAL_TEX(1) RWTexture2D<float4> GENERATION_TARGET;
BIND_MATERIAL_TEX(2) Sampler2D<float4> _SourceTex;

public struct HeightSampler : Math.IHeightSampler
{
    public float Sample(float2 uv)
    {
        return _SourceTex.SampleLevel(uv, 0.0).r;
    }
}

//$ Kernel ComputeSlopeMap

[numthreads(8, 8, 1)]
COMP void ComputeSlopeMap(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= (uint)_Resolution.x || id.y >= (uint)_Resolution.y)
            return;

    float2 uv = ((float2)id.xy + float2(0.5f)) / _Resolution;

    HeightSampler sampler = HeightSampler();

    Math.PartialDerivativeSolver<HeightSampler> solver = Math.PartialDerivativeSolver<HeightSampler>(
        uv,
        _SourceTexelSize,
        sampler);

    Math.PartialDerivative result = solver.Solve();
    result.Compress();

    GENERATION_TARGET[id.xy] = float4(result.X, result.Y, sampler.Sample(uv), 0);
}