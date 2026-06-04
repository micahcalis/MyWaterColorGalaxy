#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import Noise.Hash;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _StarSize;
    int _StarSeed;
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
}


//$ Pass Skybox
//$ Vertex SkyboxStarsVert
//$ Fragment SkyboxStarsFrag
//$ Topology Point
//$ Cull None
//$ ZWrite Off
//$ ZTest On

//$ Blend On
//$ SrcOp SrcAlpha
//$ DstOp One
//$ ColorOp Add

//$ SrcAlphaMode One
//$ DstAlphaMode One
//$ AlphaOp Add

static const float INV_REFERENCE_RES_Y = 1.0 / 1440.0f;

VERT VertexData SkyboxStarsVert(MeshData input, INSTANCEID, uint vertexID : SV_VertexID, out float pointSize : SV_PointSize)
{
    GETMODELDATA
    VertexData output;
    output.positionWS = ObjToWorld(transform, input.positionOS);
    output.positionCS = WorldToClip(output.positionWS); 
    float screenScalar = (float)_ScreenParams.y * INV_REFERENCE_RES_Y;
    float sizeHash = pow(Noise.Functions.Hash11(vertexID, _StarSeed), 0.33f);
    pointSize = _StarSize * screenScalar * (1.0f - sizeHash * 0.7f);
    return output;
}

FRAG Deferred.SkyboxFragData SkyboxStarsFrag(VertexData input, float2 pointCoord : SV_PointCoord)
{
    float dist = length(pointCoord - float2(0.5f, 0.5f));

    if (dist > 0.5f)
    {
        discard;
    }

    Deferred.SkyboxFragData fragData;
    fragData.color = _StarColor;
    fragData.normal = 0;
    fragData.waterColValues = 0;

    return fragData;
}