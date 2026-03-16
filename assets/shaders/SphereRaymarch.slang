#include "BeerMacros.slang"

import BeerCore;
import SphereTracing;
import BeerLambert;
import SDF;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _Random;
    float4 _BaseColor;
    int _ShapeIndex;
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
//$ Cull Back
//$ Blend On
//$ Zwrite On

[shader("vertex")]
VertexData SphereRaymarchVert(MeshData input)
{
    VertexData output;
    float4 positionWS = mul(_ObjToWorldMat, float4(input.positionOS, 1));
    output.positionCS = mul(_ViewProjMat, positionWS);
    output.positionWS = positionWS.xyz;
    output.transformCenter = mul(_ObjToWorldMat, float4(0, 0, 0, 1)).xyz;
    return output;
}

[shader("fragment")]
float4 SphereRaymarchFrag(VertexData input) : SV_Target
{
    float3 viewDirWS = normalize(input.positionWS - _CameraPos);
    SphereTracing.Ray ray = SphereTracing.Ray(input.positionWS, viewDirWS);
    SphereTracing.AABB boundingbox = SphereTracing.AABB(input.transformCenter, float3(2));
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

    BeerLambert lambert = BeerLambert(_BaseColor.rgb,
        trace.Position,
        trace.Normal,
        _MainLightPos);
    
    return float4(lambert.GetLambert(), 1);
}