#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import SDF;
import Galaxy.GalaxyInstancing;
import Galaxy.GalaxySDFs;
import Core.Samplers;

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
    float _GradientIntensity;
}

BIND_MATERIAL_TEX(4) Sampler3D<float2> _SunNoiseVolume;

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

public struct SunNoiseSampler : ISampler3DFloat2
{
    public float2 Sample(float3 uvw)
    {
        return _SunNoiseVolume.Sample(uvw);
    }
}

FRAG Deferred.FragData SunFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    TransformData transform = GetModelTransform(input.instanceID);
    float3 transformScale = GetTransformScale(transform.objToWorldMat);
    float3 transformCenter = GetTransformCenter(transform.objToWorldMat);
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    SphereTracing.AABB volume = SphereTracing.AABB(transformCenter, transformScale * 2);
    SphereTracing.RayAligner<SphereTracing.AABB> rayAligner = SphereTracing.RayAligner<SphereTracing.AABB>(
        volume,
        isFrontFace,
        viewDirWS);

    SphereTracing.Ray ray = rayAligner.GetAlignedRay();

    Galaxy.Sun sunSDF = Galaxy.Sun(transformCenter, 
        transformScale.x, 
        _SunColor.rgb,
        _GradientIntensity);

    SphereTracing.SphereTracer<Galaxy.Sun> tracer = SphereTracing.SphereTracer<Galaxy.Sun>(
        sunSDF,
        ray);

    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    float3 sunColor = sunSDF.GetSunColor(result.Position, 
        result.Normal,
        SunNoiseSampler());

    Deferred.DeferredShadeData shadeData = Deferred.DeferredShadeData(sunColor,
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
        Galaxy.GetTurbulence(result.Position, transformCenter, 0), 
        Galaxy.GetTremorNoise(result.Position, transformCenter, 0));

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(shadeData, watercolorData);

    return writer.Write();
}