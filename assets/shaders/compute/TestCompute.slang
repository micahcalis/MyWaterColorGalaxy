#include "BeerMacros.slang"

import BeerCore;
import Noise.PerlinNoise;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _Tiling;
    float _NoiseIntensity;
    float _NoiseExponent;
    int _Seed;
    int _ThreadGroups;
    int _Resolution;
};

struct Voxel
{
    float3 position;
    float3 color;
};

BIND_MATERIAL_TEX(1) RWTexture2D<float4> _NoiseTex;
BIND_MATERIAL_BUFFER(2) RWStructuredBuffer<Voxel> _Voxels;

//$ Kernel PerlinCompute

COMP
[numthreads(8, 8, 1)]
void PerlinCompute(uint3 id : SV_DispatchThreadID)
{
    float2 uv = float2(id.xy) / float(_ThreadGroups) + _Time * 0.1;
    Noise.Perlin perlin = Noise.Perlin(_Seed, _Tiling, _NoiseExponent, uv);
    _NoiseTex[id.xy] = (perlin.Compute(_NoiseIntensity) + 0.5);
}

//$ Kernel FunnyBallCompute

COMP
[numthreads(8, 8, 1)]
void FunnyBallCompute(uint3 id : SV_DispatchThreadID)
{
    static float radius = 0.05;
    static float offset = 0.15;
    static float decaySpeed = 0.5f;
    float2 uv = float2(id.xy) / float(_Resolution);

    float circularSpeed = _Time * 5.0f;
    float2 circularMovement = float2(sin(circularSpeed), cos(circularSpeed));
    float2 circularOffset = circularMovement * offset;

    float subCircularSpeed = _Time * 1.34f;
    float2 subCircularMovement = float2(sin(subCircularSpeed), cos(subCircularSpeed));
    float2 subCircularOffset = subCircularMovement * offset * 2;

    float2 centerUv = uv - 0.5f;
    float ballDist = distance(centerUv, circularOffset + subCircularOffset);
    float ballMask = step(ballDist, radius);
    
    float currentVal = saturate(_NoiseTex[id.xy].r);
    float decay = decaySpeed * _DeltaTime;
    float newVal = currentVal + ballMask - decay;

    _NoiseTex[id.xy] = saturate(newVal + _Voxels[0].position.x);
}