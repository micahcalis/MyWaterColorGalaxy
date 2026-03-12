#include "BeerMacros.slang"

import BeerCore;
import SphereTracing;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _SphereRadius;
    float4 _BaseColor;
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
    SphereTracing.Sphere sphere = SphereTracing.Sphere(input.transformCenter, _SphereRadius);
    SphereTracing.Ray ray = SphereTracing.Ray(input.positionWS, viewDirWS);
    SphereTracing.AABB boundingbox = SphereTracing.AABB(input.transformCenter, float3(2));

    SphereTracing.SphereTracer tracer = SphereTracing.SphereTracer(sphere, ray, boundingbox);
    bool hit = tracer.Trace();

    if (!hit)
        discard;
    
    return _BaseColor;
}