#include "Core/BeerMacros.slang"

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float2 _Resolution;
    float4 _TestColor;
}

BIND_MATERIAL_TEX(1) RWTexture2D<float4> GENERATION_TARGET;

//$ Kernel ComputePerlinWorley

COMP
[numthreads(8, 8, 1)]
void ComputePerlinWorley(uint3 id : SV_DispatchThreadID)
{
    GENERATION_TARGET[id.xy] = _TestColor;
}