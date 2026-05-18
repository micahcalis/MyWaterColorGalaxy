#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import SDF;
import Galaxy.UFOSDF;
import BeerLit;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _SaucerColor;
    float _SaucerMetallic;
    float _SaucerSmoothness;
    float4 _CockPitColor;
    float _CockPitMetallic;
    float _CockPitSmoothness;
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
//$ Vertex SaucerVert
//$ Fragment SaucerFrag
//$ Cull None
//$ Blend Off
//$ Zwrite On

VERT VertexData SaucerVert(MeshData input, INSTANCEID)
{
    GETMODELDATA
    VertexData output;
    output.positionWS = ObjToWorld(transform, input.positionOS);
    output.positionCS = WorldToClip(output.positionWS);
    return output;
}

FRAG Deferred.FragData SaucerFrag(VertexData input, bool IsFrontFace : SV_IsFrontFace)
{
    TransformData transform = GetModelTransform(input.instanceID);
    float3 center = GetTransformCenter(transform.objToWorldMat);
    float3 scale = GetTransformScale(transform.objToWorldMat);

    float radius = scale.x;
    SphereTracing.SphereVolume sphere = SphereTracing.SphereVolume(center, radius);

    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    if(!sphere.ShouldMarch(_CameraPos, IsFrontFace, _ZBufferParams.x))
    {
        discard;
    }

    SphereTracing.Ray ray = SphereTracing.Ray(_CameraPos, viewDirWS, _ZBufferParams.x);
    ray = sphere.AlignWithSphere(ray);

    if (ray.GetMaxLength() < 1e-3f)
    {
        discard;
    }

    SDF.Saucer saucerSDF = SDF.Saucer(center, scale.x * 0.8f, transform.objToWorldDirMat);

    SphereTracing.SphereTracer<SDF.Saucer> tracer = SphereTracing.SphereTracer<SDF.Saucer>(
        saucerSDF,
        ray);

    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    Deferred.DeferredShadeData data = Deferred.DeferredShadeData(_SaucerColor.rgb,
        0,
        result.Position,
        result.Normal,
        _SaucerSmoothness,
        _SaucerMetallic,
        0.5f);

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(data);

    return writer.Write();
}

//$ Pass Transparent
//$ Vertex CockpitVert
//$ Fragment CockpitFrag
//$ Cull None
//$ Blend On
//$ SrcOp SrcAlpha
//$ DstOp InvSrcAlpha
//$ ZWrite Off
//$ ZTest On

VERT VertexData CockpitVert(MeshData input, INSTANCEID)
{
    GETMODELDATA
    VertexData output;
    output.positionWS = ObjToWorld(transform, input.positionOS);
    output.positionCS = WorldToClip(output.positionWS);
    return output;
}

FRAG Deferred.TransparentFragData CockpitFrag(VertexData input, bool IsFrontFace : SV_IsFrontFace) 
{
    TransformData transform = GetModelTransform(input.instanceID);
    float3 center = GetTransformCenter(transform.objToWorldMat);
    float3 scale = GetTransformScale(transform.objToWorldMat);

    float radius = scale.x;
    SphereTracing.SphereVolume sphere = SphereTracing.SphereVolume(center, radius);

    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    if(!sphere.ShouldMarch(_CameraPos, IsFrontFace, _ZBufferParams.x))
    {
        discard;
    }
    
    SphereTracing.Ray ray = SphereTracing.Ray(_CameraPos, viewDirWS, _ZBufferParams.x);
    ray = sphere.AlignWithSphere(ray);

    if (ray.GetMaxLength() < 1e-3f)
    {
        discard;
    }

    SDF.Cockpit cockpitSDF = SDF.Cockpit(center, scale.x * 0.8f, transform.objToWorldDirMat);

    SphereTracing.SphereTracer<SDF.Cockpit> tracer = SphereTracing.SphereTracer<SDF.Cockpit>(
        cockpitSDF,
        ray);

    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    Lit.BRDF brdf = Lit.BRDF(GetMainLight(result.Position), 
        viewDirWS,
        result.Normal, 
        _CockPitColor.rgb, 
        _CockPitSmoothness,
        _CockPitMetallic);

    float3 lit = brdf.Specular() + brdf.Fresnel();

    Deferred.TransparentFragData fragData;
    fragData.color = _CockPitColor;
    fragData.depth = Deferred.WorldPosToDepth(result.Position);

    return fragData;
}