#include "BeerMacros.slang"

import BeerCore;
import BeerLit;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _BaseColor;
    float _Smoothness;
    float _Metallic;
};

BIND_MATERIAL_TEX(1) Sampler2D _AlbedoMap;

struct MeshData
{
    M_POSITION;
    M_UV;
};

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    float2 texCoord;
}

//$ Pass Opaque
//$ Vertex DefaultLitVert
//$ Fragment DefaultLitFrag
//$ Cull Back
//$ Blend On
//$ Zwrite On

VERT VertexData DefaultLitVert(MeshData input)
{
    VertexData output;
    float4 positionWS = mul(_ObjToWorldMat, float4(input.positionOS, 1));
    output.positionCS = mul(_ViewProjMat, positionWS);
    output.positionWS = positionWS.xyz;
    output.texCoord = input.texCoord;
    return output;
}

FRAG float4 DefaultLitFrag(VertexData input) : SV_Target
{
    float4 sample = _AlbedoMap.Sample(input.texCoord);

    Lit.BRDF brdf = Lit.BRDF(GetMainLight(input.positionWS), 
        GetViewDirectionWS(input.positionWS),
        float3(0, 0, 1),
        sample.rgb,
        _Smoothness,
        _Metallic);

    return float4(sample.rgb, 1);
}