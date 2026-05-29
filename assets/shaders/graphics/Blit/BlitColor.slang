#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;

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
//$ Vertex BlitColorVert
//$ Fragment BlitColorFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData BlitColorVert(MeshData input, uint vertexID : SV_VertexID)
{
    VertexData output;
    Blit.BlitTriangle blit = Blit.BlitTriangle(vertexID);
    blit.FillData(output.positionCS, output.uv);
    return output;
}

FRAG float4 BlitColorFrag(VertexData input) : SV_Target
{
    return _BlitSource.Sample(input.uv);
}