#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import SDF;
import Galaxy.GalaxyInstancing;
import Galaxy.GalaxySDFs;

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

FRAG Deferred.FragData PlanetFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    Galaxy.ObjectData materialData = Galaxy.GetMaterialData(input.instanceID);
    float3 transformCenter = Galaxy.GetTransformCenter(input.instanceID);
    float transformScale = Galaxy.GetBloatedScale(materialData.Scale, transformCenter, 1.0f);
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    SphereTracing.AABB boundingbox = SphereTracing.AABB(transformCenter, transformScale * 2);

    if (!boundingbox.ShouldMarch(_CameraPos, isFrontFace, _ZBufferParams.x))
    {
        discard;
    }

    SphereTracing.Ray ray = SphereTracing.Ray(_CameraPos, viewDirWS, _ZBufferParams.x);
    ray = boundingbox.AlignWithBox(ray);

    Galaxy.Planet planetSDF = Galaxy.Planet(transformCenter, 
        materialData.Scale);

    SphereTracing.SphereTracer<Galaxy.Planet> tracer = SphereTracing.SphereTracer<Galaxy.Planet>(
        planetSDF,
        ray);


    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;

    Deferred.DeferredShadeData data = Deferred.DeferredShadeData(materialData.ColorA.rgb,
        1,
        result.Position,
        result.Normal,
        0.5f,
        0.2f,
        0,
        0.5f);

    Deferred.DeferredWriter writer = Deferred.DeferredWriter(data);

    return writer.Write();
}