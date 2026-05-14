#include "Core/BeerMacros.slang"

module GalaxyInstancing;

namespace Galaxy
{
    public struct ObjectData
    {
        public float4 ColorA;
        public float4 ColorB;
        public float4 ColorC;
        public float4 ColorD;
        public float Scale;
        public uint Id;
    }
}

public BIND_MATERIAL_BUFFER(0) StructuredBuffer<float4> _DynamicGalaxyObjectPositions;
public BIND_MATERIAL_BUFFER(1) StructuredBuffer<Galaxy.ObjectData> _GalaxyObjectData;

namespace Galaxy
{
    public float3 ToWorldSpace(float3 positionOS, uint instanceID)
    {
        float scale = _GalaxyObjectData[instanceID].Scale;
        float3 position = _DynamicGalaxyObjectPositions[instanceID].xyz;

        return positionOS * scale + position;
    }
}