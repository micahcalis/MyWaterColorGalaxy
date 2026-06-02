#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import SDF;
import Galaxy.GalaxyInstancing;
import Galaxy.GalaxyInstancingGlobals;
import Galaxy.GalaxySDFs;
import Noise.Hash;
import Noise.FractalScale;

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

BIND_MATERIAL_TEX(4) Sampler3D _PlanetNoiseVolume;

//$ Pass Opaque
//$ Vertex PlanetVert
//$ Fragment PlanetFrag
//$ Cull None
//$ Blend Off
//$ Zwrite On

VERT VertexData PlanetVert(MeshData input, INSTANCEID)
{
    VertexData output;
    output.positionWS = Galaxy.ToWorldSpaceBloated(input.positionOS, instanceID, 1.0f);
    output.positionCS = mul(_ViewProjMat, float4(output.positionWS, 1));
    output.instanceID = instanceID;
    return output;
}

public struct PlanetNoiseReader : Galaxy.IPlanetNoiseReader
{
    public float Read(float3 uvw, int index)
    {
        index = index % 4;
        float4 sample = _PlanetNoiseVolume.Sample(uvw);
        return sample[index];
    }
}

FRAG Deferred.FragData PlanetFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    Galaxy.ObjectData materialData = Galaxy.GetMaterialData(input.instanceID);
    float3 transformCenter = Galaxy.GetTransformCenter(input.instanceID);
    float transformScale = Galaxy.GetBloatedScale(materialData.Scale, transformCenter, 1.0f);
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    SphereTracing.AABB boundingbox = SphereTracing.AABB(transformCenter, transformScale * 2);
    SphereTracing.AABB volume = SphereTracing.AABB(transformCenter, transformScale * 2);

    SphereTracing.RayAligner<SphereTracing.AABB> rayAligner = SphereTracing.RayAligner<SphereTracing.AABB>(
        volume,
        isFrontFace,
        viewDirWS);

    SphereTracing.Ray ray = rayAligner.GetAlignedRay();

    Galaxy.Planet planetSDF = Galaxy.Planet(transformCenter, 
        materialData.Scale,
        materialData.Id,
        { materialData.ColorA, materialData.ColorB, materialData.ColorC, materialData.ColorD });

    SphereTracing.SphereTracer<Galaxy.Planet> tracer = SphereTracing.SphereTracer<Galaxy.Planet>(
        planetSDF,
        ray);

    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    float hardness = Galaxy.GetScrapeNoise(result.Position, 
        transformCenter,
        result.Normal,
        viewDirWS,
        _Hardness);
        
    float4 noise = _PlanetNoiseVolume.Sample(result.Position * 0.01f);

    float3 color = planetSDF.GetColor<PlanetNoiseReader>(result.Position,
        result.Material,
        PlanetNoiseReader());

    Deferred.DeferredShadeData shadeData = Deferred.DeferredShadeData(
        color,
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