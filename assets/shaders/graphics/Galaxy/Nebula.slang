#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Nebula.NebulaUtlities;
import Nebula.NebulaTracing;
import SphereTracing;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _TestColor;
    float2 _TileDimensions;
    float _MaxRayLength;
    int _MaxRaySteps;
    float _RayStepSize;
    int _StardustCount;
    float _ShapeNoiseScale;
    float _DetailNoiseScale;
    float _Coverage;
    float _DetailIntensity;
}

BIND_MATERIAL_BUFFER(1) StructuredBuffer<float4> _DynamicStardustPositions;
BIND_MATERIAL_BUFFER(2) StructuredBuffer<ObjectData> _StardustObjectData;
BIND_MATERIAL_BUFFER(3) StructuredBuffer<Nebula.NebulaTile> _NebulaTileBuffer;
BIND_MATERIAL_TEX(4) Sampler2D<float> _DepthBuffer;
BIND_BLIT_SRC(5)
BIND_MATERIAL_TEX(6) Sampler2D<float4> _NebulaColorBuffer;
BIND_MATERIAL_TEX(7) Sampler3D<float4> _NebulaShapeNoise;
BIND_MATERIAL_TEX(8) Sampler3D<float4> _NebulaDetailNoise;
BIND_MATERIAL_TEX(9) Sampler3D<float4> _NebulaDistortionNoise;

struct MeshData
{
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float2 uv;
}

//$ Pass Volumetrics
//$ Vertex NebulaVert
//$ Fragment TraceNebulaFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData NebulaVert(MeshData input, uint vertexID : SV_VertexID)
{
    VertexData output;
    Blit.BlitTriangle blit = Blit.BlitTriangle(vertexID);
    blit.FillData(output.positionCS, output.uv);
    return output;
}

public struct NebulaObjectGetter : Nebula.INebulaObjectGetter
{
    public Nebula.ObjectPair GetObject(uint index)
    {
        Nebula.ObjectPair pair;
        pair.Position = _DynamicStardustPositions[index].xyz;
        pair.Scale = _StardustObjectData[index].Scale;
        return pair;
    }

    public Nebula.NebulaMaterial GetMaterial(uint index)
    {
        Nebula.NebulaMaterial material;
        ObjectData stardustData = _StardustObjectData[index];
        material.ColorA = stardustData.ColorA;
        material.ColorB = stardustData.ColorB;
        material.ColorC = stardustData.ColorC;
        material.ColorD = stardustData.ColorD;
        material.Seed = stardustData.Id;
        return material;
    }

    public bool IsValid(uint index)
    {
        return index < _StardustCount;
    }
}

public struct NebulaNoiseSampler : Nebula.INebulaNoiseSampler
{
    public float4 GetShape(float3 uvw, float3 offset)
    {
        float3 sampleCoord = (uvw + offset) * _ShapeNoiseScale;
        return _NebulaShapeNoise.SampleLevel(sampleCoord, 0);
    }

    public float4 GetDetail(float3 uvw, float3 offset)
    {
        float3 sampleCoord = (uvw + offset) * _DetailNoiseScale;
        return _NebulaDetailNoise.SampleLevel(sampleCoord, 0);
    }

    public float3 GetDistortion(float3 uvw, float3 offset)
    {
        return 0;
    }
}

FRAG float4 TraceNebulaFrag(VertexData input) : SV_Target
{
    uint tileID = Nebula.GetTileId(input.uv, _TileDimensions);

    Nebula.NebulaTile tile = _NebulaTileBuffer[tileID];

    if (!Nebula.ShouldTraceTile(tile))
    {
        discard;
    }

    float depth = _DepthBuffer.Sample(input.uv);
    SphereTracing.Ray ray = Nebula.GetRay(input.uv, depth, _MaxRayLength);

    Nebula.NebulaContainer<NebulaObjectGetter, NebulaNoiseSampler> container = Nebula.NebulaContainer<NebulaObjectGetter, NebulaNoiseSampler>(
        tile,
        NebulaObjectGetter(),
        NebulaNoiseSampler());

    Nebula.TraceInput traceInput;
    traceInput.MaxSteps = _MaxRaySteps;
    traceInput.StepSize = _RayStepSize;
    traceInput.Coverage = _Coverage;
    traceInput.DetailIntensity = _DetailIntensity;

    Nebula.Tracer<NebulaObjectGetter, NebulaNoiseSampler> tracer = Nebula.Tracer<NebulaObjectGetter, NebulaNoiseSampler>(
        ray,
        container,
        traceInput);

    Nebula.TraceResult result = tracer.Trace();

    if (!result.Hit)
    {
        discard;
    }

    return float4(_TestColor.rgb, 1.0f - result.Transmittance);
}

//$ Pass BlitColor
//$ Vertex NebulaVert
//$ Fragment BlendNebulaFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

FRAG float4 BlendNebulaFrag(VertexData input) : SV_Target
{
    float4 source = _BlitSource.Sample(input.uv);
    float4 nebula = _NebulaColorBuffer.Sample(input.uv);
    float3 blendColor = lerp(source.rgb, nebula.rgb, nebula.a);

    return float4(blendColor, source.a);
}
