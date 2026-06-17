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
}

BIND_MATERIAL_BUFFER(1) StructuredBuffer<float4> _DynamicStardustPositions;
BIND_MATERIAL_BUFFER(2) StructuredBuffer<ObjectData> _StardustObjectData;
BIND_MATERIAL_BUFFER(3) StructuredBuffer<Nebula.NebulaTile> _NebulaTileBuffer;

struct MeshData
{
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float2 uv;
}

//$ Pass BlitColor
//$ Vertex TraceNebulaVert
//$ Fragment TraceNebulaFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData TraceNebulaVert(MeshData input, uint vertexID : SV_VertexID)
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
}

FRAG float4 TraceNebulaFrag(VertexData input) : SV_Target
{
    uint tileID = Nebula.GetTileId(input.uv, _TileDimensions);
    Nebula.NebulaTile tile = _NebulaTileBuffer[tileID];

    if (!Nebula.ShouldTraceTile(tile))
    {
        discard;
    }

    SphereTracing.Ray ray = Nebula.GetRay(input.uv, _MaxRayLength);

    Nebula.NebulaContainer<NebulaObjectGetter> container = Nebula.NebulaContainer<NebulaObjectGetter>(tile,
        NebulaObjectGetter());

    Nebula.TraceInput input;
    input.MaxSteps = _MaxRaySteps;
    input.StepSize = _RayStepSize;

    Nebula.Tracer<NebulaObjectGetter> tracer = Nebula.Tracer<NebulaObjectGetter>(ray,
        container,
        input);

    Nebula.TraceResult result = tracer.Trace();

    if (!result.Hit)
    {
        discard;
    }

    return _TestColor;
}