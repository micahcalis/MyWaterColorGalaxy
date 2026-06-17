#include "Core/BeerMacros.slang"

import Nebula.NebulaUtlities;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float2 _TileResolution;
    int _StardustCount;
}

BIND_MATERIAL_BUFFER(1) StructuredBuffer<float4> _DynamicStardustPositions;
BIND_MATERIAL_BUFFER(2) StructuredBuffer<ObjectData> _StardustObjectData;
BIND_MATERIAL_BUFFER(3) RWStructuredBuffer<Nebula.NebulaTile> _NebulaTileBuffer;


public struct ObjectReader : Nebula.INebulaObjectReader
{
    public Nebula.ObjectPair GetObject(int id)
    {
        Nebula.ObjectPair pair;
        pair.Position = _DynamicStardustPositions[id].xyz;
        pair.Scale = _StardustObjectData[id].Scale;
        return pair;
    }
}

//$ Kernel ComputeNebulaTiles

COMP
[numthreads(8, 8, 1)]
void ComputeNebulaTiles(uint3 id : SV_DispatchThreadID)
{
    uint2 tileResolution = (uint2)ceil(_TileResolution);
    
    if (Nebula.ThreadOutOfBounds(id.xy, tileResolution))
    {
        return;
    }   

    Nebula.NebulaTileComputer<ObjectReader> tileComputer = Nebula.NebulaTileComputer<ObjectReader>(
        id.xy,
        _StardustCount,
        _TileResolution,
        ObjectReader());

    uint tileId = Nebula.FlattenTileId(id.xy, tileResolution);
    _NebulaTileBuffer[tileId] = tileComputer.Compute();
}