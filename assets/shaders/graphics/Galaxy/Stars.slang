#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import Math.Easings;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _StarSize;
    float _StarBoxSize;
    float4 _StarColor;
}

struct MeshData
{
    M_POSITION;
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    float normalizedDistance;
}

//$ Pass Transparent
//$ Vertex StarsVert
//$ Fragment StarsFrag
//$ Topology Point
//$ Cull None
//$ Blend On
//$ SrcOp One
//$ DstOp Alpha
//$ Zwrite Off

VERT VertexData StarsVert(MeshData input, out float pointSize : SV_PointSize)
{
    VertexData output;

    float3 relativePos = input.positionOS - _CameraPos;
    float halfBox = _StarBoxSize * 0.5f;
    float3 wrappedPos = relativePos + halfBox;
    wrappedPos = wrappedPos - _StarBoxSize * floor(wrappedPos / _StarBoxSize);
    wrappedPos -= halfBox;

    output.positionWS = _CameraPos + wrappedPos;
    output.positionCS = WorldToClip(output.positionWS);
     
    float dist = distance(output.positionWS, _CameraPos);
    pointSize = _StarSize / dist;
    output.normalizedDistance = dist / length(float3(halfBox));

    return output;
}

FRAG float4 StarsFrag(VertexData input, float2 pointCoord : SV_PointCoord) : SV_Target
{
    float dist = length(pointCoord - float2(0.5f, 0.5f));

    if (dist > 0.5f)
    {
        discard;
    }
    
    // Deferred.DeferredShadeData data = Deferred.DeferredShadeData(_StarColor.rgb,
    // 1,
    // input.positionWS,
    // 1,
    // 0,
    // 0,
    // 0);

    // Deferred.DeferredWriter writer = Deferred.DeferredWriter(data);

    float nearMask = 1.0f - smoothstep(0.08f, 0.02f, input.normalizedDistance);
    float alpha = pow(1.0f - input.normalizedDistance, 2) * nearMask;

    return alpha * _StarColor;
}