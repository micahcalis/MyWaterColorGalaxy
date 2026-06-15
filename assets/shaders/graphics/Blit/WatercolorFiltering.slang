#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Blit.PostProcessing;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _Vibrance;
};

BIND_BLIT_SRC(1)

struct MeshData
{
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float2 uv;
}

//$ Pass BlitColor
//$ Vertex WatercolorFilteringVert
//$ Fragment WatercolorFilteringFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData WatercolorFilteringVert(MeshData input, uint vertexID : SV_VertexID)
{
    VertexData output;
    Blit.BlitTriangle blit = Blit.BlitTriangle(vertexID);
    blit.FillData(output.positionCS, output.uv);
    return output;
}

FRAG float4 WatercolorFilteringFrag(VertexData input) : SV_Target
{
    float4 sample = _BlitSource.Sample(input.uv);
    
    float3 ppColor = sample.rgb;

    ppColor = Color.ApplyVibrance(ppColor, _Vibrance);

    return float4(ppColor, sample.a);
}