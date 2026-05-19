#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import BeerLit;
import Deferred.DeferredUtils;

// Micah: here are the material properties
BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _BaseColor;
    float _Smoothness;
    float _Metallic;
};

// Micah: we can bind material textures / ssbo's
BIND_MATERIAL_TEX(1) Sampler2D _AlbedoMap;

struct MeshData
{
    M_POSITION;
    M_NORMAL;
    M_UV;
};

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    float3 normalWS;
    float2 texCoord;
}

// Micah: these are the pass settings, exported to JSON
//$ Pass Opaque
//$ Vertex DefaultLitVert
//$ Fragment DefaultLitFrag
//$ Cull Back
//$ Blend On
//$ Zwrite On

VERT VertexData DefaultLitVert(MeshData input, INSTANCEID)
{
    // Micah: helper Macro to get transform data
    GETMODELDATA
    VertexData output;
    float4 positionWS = mul(transform.objToWorldMat, float4(input.positionOS, 1));
    output.positionCS = mul(_ViewProjMat, positionWS);
    output.positionWS = positionWS.xyz;
    output.normalWS = mul(transform.objToWorldDirMat, float4(input.normalOS, 1)).xyz;
    output.texCoord = input.texCoord;
    return output;
}

FRAG Deferred.FragData DefaultLitFrag(VertexData input)
{
    float4 sample = _AlbedoMap.Sample(input.texCoord);
    float3 albedo = sample.rgb * _BaseColor.rgb;

    Deferred.DeferredShadeData shadeData = Deferred.DeferredShadeData(_BaseColor.rgb,
        1,
        input.positionWS,
        input.normalWS,
        _Smoothness,
        _Metallic,
        0,
        0.5f);

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(shadeData);

    return writer.Write();
}