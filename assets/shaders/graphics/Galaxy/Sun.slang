#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import SDF;
import Galaxy.GalaxyInstancing;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _SunColor;
    float _Smoothness;
    float _Hardness;
    float _Wetness;
    float _GranulationNoiseIntensity;
    float _DepthBleedMin;
    float _DepthBleedMax;
}

struct MeshData
{
    M_POSITION;
}

struct VertexData   
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    nointerpolation uint instanceID;
}

//$ Pass Opaque
//$ Vertex SunVert
//$ Fragment SunFrag
//$ Cull None
//$ Blend Off
//$ Zwrite On

VERT VertexData SunVert(MeshData input, INSTANCEID)
{
    TransformData transform = GetModelTransform(instanceID);
    VertexData output;
    output.positionWS = ObjToWorld(transform, input.positionOS);
    output.positionCS = WorldToClip(output.positionWS);
    return output;
}

FRAG Deferred.FragData SunFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    TransformData transform = GetModelTransform(input.instanceID);
    float3 transformScale = GetTransformScale(transform.objToWorldMat);
    float3 transformCenter = GetTransformCenter(transform.objToWorldMat);
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    SphereTracing.AABB boundingbox = SphereTracing.AABB(transformCenter, transformScale * 2);

    if (!boundingbox.ShouldMarch(_CameraPos, isFrontFace, _ZBufferParams.x))
    {
        discard;
    }

    SphereTracing.Ray ray = SphereTracing.Ray(_CameraPos, viewDirWS, _ZBufferParams.x);
    ray = boundingbox.AlignWithBox(ray);

    SDF.Sphere planetSDF = SDF.Sphere(transformCenter, transformScale.x * 0.8f);

    SphereTracing.SphereTracer<SDF.Sphere> tracer = SphereTracing.SphereTracer<SDF.Sphere>(
        planetSDF,
        ray);

    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    Deferred.DeferredShadeData shadeData = Deferred.DeferredShadeData(_SunColor.rgb,
        1,
        result.Position,
        result.Normal,
        _Smoothness,
        0.2f,
        0,
        0.5f,
        0.0f);

    Deferred.DeferredWatercolorData watercolorData = Deferred.DeferredWatercolorData(
        Galaxy.GetDistortion(_Wetness), 
        Galaxy.GetGranulation(result.Position, transformCenter, result.Normal, _GranulationNoiseIntensity), 
        Galaxy.GetColorBleed(result.Position, result.Normal, _DepthBleedMin, _DepthBleedMax), 
        Galaxy.GetTurbulence(result.Position, transformCenter), 
        Galaxy.GetTremorNoise(result.Position, transformCenter));

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(shadeData, watercolorData);

    return writer.Write();
}