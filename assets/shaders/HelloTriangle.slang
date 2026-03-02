#include "BeerMacros.slang"

import BeerCore;
import ColorSpace;

struct MeshData
{
    M_POSITION;
    M_UV;
    M_COLOR;
};

struct VertexData 
{
    float4 positionCS : SV_Position;
    float2 texCoord;
    float4 color;
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
    return output;
}

[shader("fragment")]
float4 HelloTriangleFrag(VertexData input) : SV_Target
{
    Color.ColorTraveler traveler = Color.ColorTraveler(float3(1, 0, 0));
    traveler.ToggleSpace();
    traveler.Color.x = frac(traveler.Color.x + _Time * 2);
    traveler.ToggleSpace();
    
    return float4(traveler.Color, 1);
}