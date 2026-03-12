#include "BeerMacros.slang"

import BeerCore;
import ColorSpace;
import BeerLambert;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _Test;
    float4 _BaseColor;
};

BIND_MATERIAL_TEX(1) Sampler2D _MainTex;

struct MeshData
{
    M_POSITION;
    M_UV;
    M_COLOR;
    M_NORMAL;
};

struct VertexData 
{
    float4 positionCS : SV_Position;
    float2 texCoord;
    float4 color;
    float3 normalOS;
    float3 positionWS;
};

//$ Pass Opaque
//$ Vertex HelloTriangleVert
//$ Fragment HelloTriangleFrag
//$ Cull Back
//$ Blend On
//$ Zwrite On

[shader("vertex")]
VertexData HelloTriangleVert(MeshData input)
{
    VertexData output;
    float4 positionWS = float4(input.positionOS, 1);
    output.positionCS = mul(_ViewProjMat, positionWS);
    output.color = input.color;
    output.texCoord = input.texCoord;
    output.normalOS = input.normalOS;
    output.positionWS = input.positionOS;
    return output;
}

[shader("fragment")]
float4 HelloTriangleFrag(VertexData input) : SV_Target
{
    Color.ColorTraveler traveler = Color.ColorTraveler(float3(0.8, 0.2, 0));
    traveler.ToggleSpace();
    traveler.Color.r = frac(traveler.Color.r + _Time * 0.2);
    traveler.ToggleSpace();

    float4 sample = _MainTex.Sample(input.texCoord);
    BeerLambert lambert = BeerLambert(traveler.Color, input.positionWS, input.normalOS, float3(0, 100, 10));
    
    return float4(lambert.GetLambert(), 1);
}