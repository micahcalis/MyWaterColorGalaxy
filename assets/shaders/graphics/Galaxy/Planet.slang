#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import SDF;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _ColorA;
    float4 _ColorB;
    float4 _ColorC;
    float4 _ColorD;
    float _Radius;
}

struct MeshData
{
    M_POSITION;
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    float3 transformCenter;
}

//$ Pass Opaque
//$ Vertex PlanetVert
//$ Fragment PlanetFrag
//$ Cull None
//$ Blend Off
//$ Zwrite On

VERT VertexData PlanetVert(MeshData input, INSTANCEID)
{
    GETMODELDATA
    VertexData output;
    float4 positionWS = mul(transform.objToWorldMat, float4(input.positionOS, 1));
    output.positionCS = mul(_ViewProjMat, positionWS);
    output.positionWS = positionWS.xyz;
    output.transformCenter = mul(transform.objToWorldMat, float4(0, 0, 0, 1)).xyz;
    return output;
}

FRAG Deferred.FragData PlanetFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);
    SphereTracing.AABB boundingbox = SphereTracing.AABB(input.transformCenter, float3(2));

    if (!boundingbox.ShouldMarch(_CameraPos, isFrontFace, _ZBufferParams.x))
    {
        discard;
    }

    SphereTracing.Ray ray = SphereTracing.Ray(_CameraPos, viewDirWS, _ZBufferParams.x);
    ray = boundingbox.AlignWithBox(ray);

    SDF.Sphere planetSDF = SDF.Sphere(input.transformCenter, _Radius);

    SphereTracing.SphereTracer<SDF.Sphere> tracer = SphereTracing.SphereTracer<SDF.Sphere>(
        planetSDF,
        ray,
        boundingbox);

    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    Deferred.DeferredShadeData data = Deferred.DeferredShadeData(_ColorA.rgb,
        0,
        result.Position,
        result.Normal,
        0.5f,
        0.2f,
        0.5f);

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(data);

    return writer.Write();
}