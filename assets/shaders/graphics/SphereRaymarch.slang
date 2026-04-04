#include "BeerMacros.slang"

import BeerCore;
import SphereTracing;
import BeerLit;
import SDF;
import ColorSpace;
import Deferred.DeferredUtils;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _Random;
    float4 _BaseColor;
    int _ShapeIndex;
    float _Smoothness;
    float _Metallic;
};

struct MeshData
{
    M_POSITION;
};

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    float3 transformCenter;
};

//$ Pass Opaque
//$ Vertex SphereRaymarchVert
//$ Fragment SphereRaymarchFrag
//$ Cull None
//$ Blend Off
//$ Zwrite On

VERT VertexData SphereRaymarchVert(MeshData input, INSTANCEID)
{
    GETMODELDATA
    VertexData output;
    float4 positionWS = mul(transform.objToWorldMat, float4(input.positionOS, 1));
    output.positionCS = mul(_ViewProjMat, positionWS);
    output.positionWS = positionWS.xyz;
    output.transformCenter = mul(transform.objToWorldMat, float4(0, 0, 0, 1)).xyz;
    return output;
}

FRAG Deferred.FragData SphereRaymarchFrag(VertexData input, bool isFrontFace : SV_IsFrontFace)
{
    Deferred.FragData frag;

    float3 viewDirWS = GetViewDirectionWS(input.positionWS);
    SphereTracing.AABB boundingbox = SphereTracing.AABB(input.transformCenter, float3(2));

    if (!boundingbox.ShouldMarch(_CameraPos, isFrontFace, _ZBufferParams.x))
    {
        discard;
    }

    SphereTracing.Ray ray = SphereTracing.Ray(_CameraPos, viewDirWS, _ZBufferParams.x);
    ray = boundingbox.AlignWithBox(ray);

    SphereTracing.TraceResult trace = SphereTracing.TraceResult(input.positionWS);

    switch (_ShapeIndex)
    {
        case 0:
        {
            SDF.Sphere shape = SDF.Sphere(input.transformCenter, _Random.x);
            SphereTracing.SphereTracer<SDF.Sphere> tracer = 
                SphereTracing.SphereTracer<SDF.Sphere>(shape, ray, boundingbox);
                
            trace = tracer.Trace();
            break;
        }
        case 1:
        {
            SDF.Box shape = SDF.Box(input.transformCenter, _Random.xyz);
            SphereTracing.SphereTracer<SDF.Box> tracer = 
                SphereTracing.SphereTracer<SDF.Box>(shape, ray, boundingbox);
                
            trace = tracer.Trace();
            break;
        }
        case 2:
        {
            SDF.Torus shape = SDF.Torus(input.transformCenter, _Random.x, _Random.y);
            SphereTracing.SphereTracer<SDF.Torus> tracer = 
                SphereTracing.SphereTracer<SDF.Torus>(shape, ray, boundingbox);
                
            trace = tracer.Trace();
            break;
        }
        case 3:
        {
            SDF.OctaHedron shape = SDF.OctaHedron(input.transformCenter, _Random.x);
            SphereTracing.SphereTracer<SDF.OctaHedron> tracer = 
                SphereTracing.SphereTracer<SDF.OctaHedron>(shape, ray, boundingbox);
                
            trace = tracer.Trace();
            break;
        }
        default:
        {
            SDF.Sphere shape = SDF.Sphere(input.transformCenter, _Random.x);
            SphereTracing.SphereTracer<SDF.Sphere> tracer = 
                SphereTracing.SphereTracer<SDF.Sphere>(shape, ray, boundingbox);
                
            trace = tracer.Trace();
            break;
        }
    }

    if (!trace.Hit)
        discard;

    Color.ColorTraveler traveler = Color.ColorTraveler(_BaseColor.rgb);
    traveler.ToggleSpace();

    Lit.BRDF brdf = Lit.BRDF(GetMainLight(trace.Position), 
        viewDirWS,
        trace.Normal,
        traveler.Color,
        _Smoothness,
        _Metallic);

    frag.color = float4(brdf.Shade(), 1);
    frag.depth = Deferred.WorldPosToDepth(trace.Position);

    return frag;
}