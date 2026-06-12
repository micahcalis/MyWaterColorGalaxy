#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _TransitionTime;
}

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
//$ Vertex SpaceTransitionVert
//$ Fragment SpaceTransitionFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData SpaceTransitionVert(MeshData input, uint vertexID : SV_VertexID)
{
    VertexData output;
    Blit.BlitTriangle blit = Blit.BlitTriangle(vertexID);
    blit.FillData(output.positionCS, output.uv);
    return output;
}

FRAG float4 SpaceTransitionFrag(VertexData input) : SV_Target
{
    float4 sample = _BlitSource.Sample(input.uv);

    return lerp(sample, 0, saturate(_TransitionTime));
}