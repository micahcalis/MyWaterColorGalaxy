#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import SphereTracing;
import Galaxy.UFOSDF;
import Galaxy.GalaxyInstancing;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _InnerFlameColor;
    float4 _OuterFlameColor;
    float _Wetness;
    float _GranulationNoiseIntensity;
    int _BoostState;
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
    
//$ Pass Transparent
//$ Vertex BoostFlameVert
//$ Fragment BoostFlameFrag
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

VERT VertexData BoostFlameVert(MeshData input, INSTANCEID)
{
    GETMODELDATA
    VertexData output;
    output.positionOS = input.positionOS;
    output.positionWS = ObjToWorld(transform, input.positionOS);
    output.positionCS = WorldToClip(output.positionWS);
    return output;
}

FRAG Deferred.TransparentFragData BoostFlameFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    bool isBoosting = _BoostState != 0;

    if (!isBoosting)
    {
        discard;
    }

    TransformData transform = GetModelTransform(input.instanceID);
    float3 center = GetTransformCenter(transform.objToWorldMat);
    float3 scale = GetTransformScale(transform.objToWorldMat);
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    SphereTracing.OBB volume = SphereTracing.OBB(center, 
        scale * 2.0f, 
        (float3x3)transform.objToWorldDirMat);

    SphereTracing.RayAligner<SphereTracing.OBB> rayAligner = SphereTracing.RayAligner<SphereTracing.OBB>(
        volume,
        isFrontFace,
        viewDirWS);

    SphereTracing.Ray ray = rayAligner.GetAlignedRay();

    SDF.Flame flameSDF = SDF.Flame(center,
        scale.x,
        transform.objToWorldDirMat,
        _InnerFlameColor,
        _OuterFlameColor);

    SphereTracing.SphereTracer<SDF.Flame> tracer = SphereTracing.SphereTracer<SDF.Flame>(
        flameSDF,
        ray);
    
    SphereTracing.TraceResult result = tracer.Trace();

    if (!result.Hit)
        discard;
    
    Deferred.TransparentFragData fragData = (Deferred.TransparentFragData)0;
    fragData.color = flameSDF.GetColor(viewDirWS, result.Normal);
    fragData.depth = Deferred.WorldPosToDepth(result.Position);

    return fragData;
}