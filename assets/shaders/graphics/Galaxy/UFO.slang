#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import Galaxy.GalaxyInstancing;
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
    float _CutoffTime;
    float4 _CutoffColor;
    float _GlowThickness;
    float _Hardness;
    float _Wetness;
    float _GranulationNoiseIntensity;
}

struct MeshData
{
    M_POSITION; 
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    float3 positionOS;
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
    output.positionOS = input.positionOS;
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

    SDF.Saucer saucerSDF = SDF.Saucer(center, scale.x * 0.8f, transform.objToWorldDirMat, _CutoffTime);

    SphereTracing.SphereTracer<SDF.Saucer> tracer = SphereTracing.SphereTracer<SDF.Saucer>(
        saucerSDF,
        ray);

    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    float3x3 invRotation = (float3x3)transpose(transform.objToWorldDirMat);
    float3 positionOS = mul(invRotation, (result.Position - center) / scale);
    float glowGradient = SDF.GetGlowGradient(positionOS, _CutoffTime, _GlowThickness);

    float hardness = Galaxy.GetScrapeNoise(result.Position, 
        center,
        result.Normal,
        viewDirWS,
        _Hardness);
 
    Deferred.DeferredShadeData shadeData = Deferred.DeferredShadeData(_SaucerColor.rgb,
        2,
        result.Position,
        result.Normal,
        _SaucerSmoothness,
        _SaucerMetallic,
        _CutoffColor.rgb * glowGradient,
        0.5f,
        hardness);

    Deferred.DeferredWatercolorData watercolorData = Deferred.DeferredWatercolorData(
        Galaxy.GetDistortion(_Wetness), 
        Galaxy.GetGranulation(result.Position, center, result.Normal, _GranulationNoiseIntensity), 
        Galaxy.GetColorBleed(result.Position, result.Normal, 1000, 10000), 
        Galaxy.GetTurbulence(result.Position, center), 
        Galaxy.GetTremorNoise(result.Position, center));

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(shadeData, watercolorData);

    return writer.Write();
}

//$ Pass Transparent
//$ Vertex CockpitVert
//$ Fragment CockpitFrag
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

    SDF.Cockpit cockpitSDF = SDF.Cockpit(center, 
        scale.x * 0.8f, 
        transform.objToWorldDirMat,
       _CutoffTime);

    SphereTracing.SphereTracer<SDF.Cockpit> tracer = SphereTracing.SphereTracer<SDF.Cockpit>(
        cockpitSDF,
        ray);

    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    float3x3 invRotation = (float3x3)transpose(transform.objToWorldDirMat);
    float3 positionOS = mul(invRotation, (result.Position - center) / scale);
    float glowGradient = SDF.GetGlowGradient(positionOS, _CutoffTime, _GlowThickness);
    float4 gradientCol = glowGradient * _CutoffColor;

    Lit.BRDF brdf = Lit.BRDF(GetMainLight(result.Position), 
        viewDirWS,
        result.Normal, 
        _CockPitColor.rgb, 
        _CockPitSmoothness,
        _CockPitMetallic);

    float3 lit = brdf.Lambert() + brdf.Specular() + brdf.Fresnel();
    float alpha = saturate(_CockPitColor.a + glowGradient);
    float3 blendCol = lit + gradientCol.rgb;

    Deferred.TransparentFragData fragData;
    fragData.color = float4(blendCol * alpha, alpha);

    fragData.normal = Deferred.PackTransparentNormalOffsetData(Galaxy.GetTremorNoise(result.Position, center)) * alpha;

    fragData.waterColValues = Deferred.PackTransparentWatercolorData(Galaxy.GetDistortion(_Wetness), 
        Galaxy.GetGranulation(result.Position, center, result.Normal, _GranulationNoiseIntensity), 
        Galaxy.GetColorBleed(result.Position, result.Normal, 0, 0.01f)) * alpha;

    fragData.depth = Deferred.WorldPosToDepth(result.Position);

    return fragData;
}