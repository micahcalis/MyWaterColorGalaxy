#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Galaxy.GalaxyInstancing;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import SDF;
import Galaxy.GalaxyInstancingGlobals;
import Galaxy.GalaxySDFs;

struct MeshData
{
    M_POSITION;
    M_COLOR;
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    nointerpolation uint instanceID;
    nointerpolation uint asteroidID;
    nointerpolation float3 transformCenter;
    nointerpolation float transformScale;
}

BIND_MATERIAL_TEX(4) Sampler3D<float> _AsteroidNoiseVolume;

//$ Pass Opaque
//$ Vertex AsteroidsVert
//$ Fragment AsteroidsFrag
//$ Cull None
//$ Blend Off
//$ Zwrite On

VERT VertexData AsteroidsVert(MeshData input, uint vertexID : SV_VertexID, INSTANCEID)
{
    VertexData output;
    output.instanceID = instanceID;
    output.asteroidID = (uint)round(input.color.r);

    output.positionWS = Galaxy.ToWorldSpaceBloatedAsteroid(input.positionOS,
        instanceID,
        1.0f,
        output.asteroidID,
        output.transformCenter,
        output.transformScale);

    output.positionCS = WorldToClip(output.positionWS);
    return output;
}

public struct AsteroidNoiseReader : Galaxy.IAsteroidNoiseReader
{
    public float Read(float3 uvw)
    {
        return _AsteroidNoiseVolume.Sample(uvw);
    }
}

FRAG Deferred.FragData AsteroidsFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    Galaxy.ObjectData materialData = Galaxy.GetMaterialData(input.instanceID);
    float3 transformCenter = input.transformCenter;
    float transformScale = input.transformScale;
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    SphereTracing.AABB boundingbox = SphereTracing.AABB(transformCenter, transformScale * 2);
    SphereTracing.AABB volume = SphereTracing.AABB(transformCenter, transformScale * 2);

    SphereTracing.RayAligner<SphereTracing.AABB> rayAligner = SphereTracing.RayAligner<SphereTracing.AABB>(
        volume,
        isFrontFace,
        viewDirWS);

    SphereTracing.Ray ray = rayAligner.GetAlignedRay();  

    Galaxy.Asteroid<AsteroidNoiseReader> asteroidsSDF = Galaxy.Asteroid<AsteroidNoiseReader>(
        AsteroidNoiseReader(),
        transformCenter, 
        transformScale,
        materialData.ColorA.rgb,
        input.asteroidID,
        materialData.Id);

    SphereTracing.SphereTracer<Galaxy.Asteroid<AsteroidNoiseReader>> tracer = SphereTracing.SphereTracer<Galaxy.Asteroid<AsteroidNoiseReader>>(
        asteroidsSDF,
        ray);

    SphereTracing.TraceResult result = tracer.Trace(0.9f);

    if (!result.Hit)
        discard;

    float hardness = Galaxy.GetScrapeNoise(result.Position, 
        transformCenter,
        result.Normal,
        viewDirWS,
        _Hardness);

    Deferred.DeferredShadeData shadeData = Deferred.DeferredShadeData(
        asteroidsSDF.GetColor({materialData.ColorA.rgb, materialData.ColorB.rgb, materialData.ColorC.rgb, materialData.ColorD.rgb}),
        2,
        result.Position,
        result.Normal,
        _Smoothness,
        0.0f,
        0,
        0.0f,
        hardness);

    Deferred.DeferredWatercolorData watercolorData = Deferred.DeferredWatercolorData(
        Galaxy.GetDistortion(_Wetness), 
        Galaxy.GetGranulation(result.Position, transformCenter, result.Normal, _GranulationNoiseIntensity), 
        Galaxy.GetColorBleed(result.Position, result.Normal, _DepthBleedMin, _DepthBleedMax), 
        Galaxy.GetTurbulence(result.Position, transformCenter, input.instanceID), 
        Galaxy.GetTremorNoise(result.Position, transformCenter, input.instanceID));

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(shadeData, watercolorData);

    return writer.Write();
}

