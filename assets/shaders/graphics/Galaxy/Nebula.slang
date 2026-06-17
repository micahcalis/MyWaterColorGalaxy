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

    Nebula.NebulaContainer<NebulaObjectGetter> container = Nebula.NebulaContainer<NebulaObjectGetter>(tile,
        NebulaObjectGetter());

    Nebula.TraceInput traceInput;
    traceInput.MaxSteps = _MaxRaySteps;
    traceInput.StepSize = _RayStepSize;

    Nebula.Tracer<NebulaObjectGetter> tracer = Nebula.Tracer<NebulaObjectGetter>(ray,
        container,
        traceInput);

    Nebula.TraceResult result = tracer.Trace();

    if (!result.Hit)
    {
        discard;
    }

    float3 debugVolume = _NebulaShapeNoise.SampleLevel(0, 0).rgb * 0.01f 
        + _NebulaDetailNoise.SampleLevel(0, 0).rgb * 0.01f
        + _NebulaDistortionNoise.SampleLevel(0, 0).rgb * 0.01f;

    return _TestColor + debugVolume.r;
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
