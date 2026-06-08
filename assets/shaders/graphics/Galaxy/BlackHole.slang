#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Galaxy.GalaxyInstancing;
import Galaxy.GalaxyInstancingGlobals;
import Galaxy.GalaxySDFs;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;

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
//$ Vertex BlackHoleVert
//$ Fragment BlackHoleFrag
//$ Cull None
//$ Blend Off
//$ Zwrite On

VERT VertexData BlackHoleVert(MeshData input, INSTANCEID)
{
    VertexData output;
    output.positionWS = Galaxy.ToWorldSpace(input.positionOS, instanceID);
    output.positionCS = WorldToClip(output.positionWS);
    output.instanceID = instanceID;
    return output;
}

FRAG Deferred.FragData BlackHoleFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    Galaxy.ObjectData materialData = Galaxy.GetMaterialData(input.instanceID);
    float3 transformCenter = Galaxy.GetTransformCenter(input.instanceID);
    float transformScale = materialData.Scale;
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);   

    SphereTracing.AABB volume = SphereTracing.AABB(transformCenter, transformScale * 2);

    SphereTracing.RayAligner<SphereTracing.AABB> rayAligner = SphereTracing.RayAligner<SphereTracing.AABB>(
        volume,
        isFrontFace,
        viewDirWS);

    SphereTracing.Ray ray = rayAligner.GetAlignedRay();

    Galaxy.BlackHole blackHoleSDF = Galaxy.BlackHole(transformCenter,
        transformScale,
        materialData.Id,
        {materialData.ColorA.rgb, materialData.ColorB.rgb, materialData.ColorC.rgb, materialData.ColorD.rgb});

    SphereTracing.SphereTracer<Galaxy.BlackHole> tracer = SphereTracing.SphereTracer<Galaxy.BlackHole>(
        blackHoleSDF,
        ray);
    
    SphereTracing.TraceResult result = tracer.Trace(0.8f, 1.0f);

    if (!result.Hit)
        discard;

    Deferred.DeferredShadeData shadeData = Deferred.DeferredShadeData(
        blackHoleSDF.GetColor(result.Position, result.Material),
        1,
        result.Position,
        result.Normal,
        _Smoothness,
        0.0f,
        0,
        0.0f,
        0.0f);

    Deferred.DeferredWatercolorData watercolorData = Deferred.DeferredWatercolorData(
        Galaxy.GetDistortion(_Wetness), 
        Galaxy.GetGranulation(result.Position, transformCenter, result.Normal, _GranulationNoiseIntensity), 
        Galaxy.GetColorBleed(result.Position, result.Normal, _DepthBleedMin, _DepthBleedMax), 
        Galaxy.GetTurbulence(result.Position, transformCenter, input.instanceID), 
        Galaxy.GetTremorNoise(result.Position, transformCenter, input.instanceID));

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(shadeData, watercolorData);

    return writer.Write();
}