#include "BeerMacros.slang"

import BeerCore;

struct MeshData
{
    M_POSITION;
    M_NORMAL;
};

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    float3 normalWS;
}

//$ Pass Opaque
//$ Vertex SkyboxVert
//$ Fragment SkyboxFrag
//$ Cull Back
//$ Blend On
//$ Zwrite On

struct Particle
{
    float3 pos;
};

BIND_MATERIAL_BUFFER(1) StructuredBuffer<Particle> _ParticleBuffer;

VERT VertexData SkyboxVert(MeshData input, INSTANCEID)
{
    VertexData output;
    float3 basePos = input.positionOS;
    float3 scaledPos = basePos * 1.0f;
    float3 positionWS = _ParticleBuffer[instanceID].pos + scaledPos;

    output.positionCS = mul(_ViewProjMat, float4(positionWS, 1));
    output.positionWS = positionWS.xyz;
    output.normalWS = input.normalOS;

    return output;
}

FRAG float4 SkyboxFrag(VertexData input) : SV_Target
{
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);
    float fresnel = saturate(dot(input.normalWS, -viewDirWS));
    return float4(float3(0.788, 0.784, 0.565) * fresnel, 1);
}
