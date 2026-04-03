#include "BeerMacros.slang"

import BeerCore;

struct Particle
{
    float3 pos;
};

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    int _ParticleCount;
    float _Speed;
    float _MaxRadius;
};

BIND_MATERIAL_BUFFER(1) RWStructuredBuffer<Particle> _ParticleBuffer;

float Hash(uint n) 
{
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return float(n & 0x7fffffffU) / float(0x7fffffff);
}

//$ Kernel UpdateParticleCompute

COMP
[numthreads(32, 1, 1)]
void UpdateParticleCompute(uint3 id: SV_DispatchThreadID)
{
    static float3 center = float3(-10, 0, -10);

    uint particleId = id.x;

    if (particleId >= _ParticleCount)
        return;
    
    static float maxHeight = 20.0f;

    float normalizedId = (float)particleId / (float)_ParticleCount;
    float baseHeight = normalizedId * maxHeight;
    float height = fmod(baseHeight + _Time * _Speed, maxHeight);

    float randomAngle = Hash(particleId) * 6.28318530718f;
    float currentAngle = randomAngle + (_Time * _Speed);

    float2 circularOffset = float2(cos(currentAngle), sin(currentAngle));
    
    circularOffset *= (height / maxHeight) * _MaxRadius;

    _ParticleBuffer[particleId].pos = float3(circularOffset.x, height, circularOffset.y) + center;
}