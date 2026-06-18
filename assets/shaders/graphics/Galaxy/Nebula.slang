#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Nebula.NebulaUtlities;
import Nebula.NebulaTracing;
import SphereTracing;
import Noise.Hash;

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
    float _DistortionNoiseScale;
    float _ColorNoiseScale;
    float _Coverage;
    float _DetailIntensity;
    float _ShapeAbsorption;
    float _LightAbsorption;
    float _HenyeyG;
    float _HenyeyK;
    float _LightMultiplier;
    float _DistortionIntensity;
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
        pair.Id = _StardustObjectData[index].Id;
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
    public float4 GetShape(float3 uvw, float3 offset, int id)
    {
        offset += GetOffsetHash(id, 493);
        float3 scale = _ShapeNoiseScale * GetScaleHash(id, 292);
        float3 sampleCoord = (uvw + offset) * scale;
        return _NebulaShapeNoise.SampleLevel(sampleCoord, 0);
    }

    public float4 GetDetail(float3 uvw, float3 offset, int id)
    {
        offset += GetOffsetHash(id, 89312);
        float3 scale = _DetailNoiseScale * GetScaleHash(id, 594);
        float3 sampleCoord = (uvw + offset) * scale;
        return _NebulaDetailNoise.SampleLevel(sampleCoord, 0);
    }

    public float3 GetDistortion(float3 uvw, float3 offset, int id)
    {
        offset += GetOffsetHash(id, 3);
        float3 scale = _DistortionNoiseScale * GetScaleHash(id, 22);
        float3 sampleCoord = (uvw + offset) * scale;
        return _NebulaDistortionNoise.SampleLevel(sampleCoord, 0).rgb * 2.0f - 1.0f;
    }

    public float3 GetScaleHash(int id, int seed)
    {
        float3 rawHash = Noise.Functions.Hash13(id, seed);
        return ((rawHash - 0.5f) * 0.2f) + 1.0f;
    }

    public float3 GetOffsetHash(int id, int seed)
    {
        float3 rawHash = Noise.Functions.Hash13(id, seed);
        return rawHash * 100;
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

    Nebula.TraceInput traceInput = Nebula.TraceInput(_MaxRaySteps,
        _RayStepSize,
        _Coverage,
        _DetailIntensity,
        _ShapeAbsorption,
        _LightAbsorption,
        _LightMultiplier,
        _DistortionIntensity,
        _HenyeyG,
        _HenyeyK,
        ray.GetDirection(),
        normalize(_MainLightPos - ray.GetOrigin()));

    Nebula.Tracer<NebulaObjectGetter, NebulaNoiseSampler> tracer = Nebula.Tracer<NebulaObjectGetter, NebulaNoiseSampler>(
        ray,
        container,
        traceInput);

    Nebula.TraceResult result = tracer.Trace();

    if (!result.Hit)
    {
        discard;
    }

    float alpha = saturate(1.0f - result.Transmittance);
    alpha += _NebulaDistortionNoise.SampleLevel(0, 0).a * 0.01f;

    return float4(result.Color, alpha);
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
    float3 blendColor = (source.rgb * (1.0f - nebula.a)) + nebula.rgb;

    return float4(blendColor, source.a);
}
