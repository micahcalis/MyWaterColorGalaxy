#include "Core/BeerMacros.slang"

import Core.BeerCore;

module Beer3D;

BIND_MODEL_PUSH
public cbuffer ModelParams
{
    public row_major float3x4 _ObjToWorldMat;
    public row_major float3x4 _ObjToWorldDirMat;
    public int _IsInstanced;
};

public struct TransformData
{
    public float4x4 objToWorldMat;
    public float4x4 objToWorldDirMat;
}

BIND_INSTANCING_BUFFER StructuredBuffer<TransformData> _TransformInstancingBuffer;

float4x4 CompleteModelTransform(float3x4 transform)
{
    return float4x4(transform[0],
            transform[1],
            transform[2],
            float4(0.0f, 0.0f, 0.0f, 1.0f));
}

public TransformData GetModelTransform(uint instanceId)
{
    TransformData transformData;

    if (_IsInstanced == 0)
    {
        transformData.objToWorldMat = CompleteModelTransform(_ObjToWorldMat);
        transformData.objToWorldDirMat = CompleteModelTransform(_ObjToWorldDirMat);
    }
    else
    {
        transformData = _TransformInstancingBuffer[instanceId];
    }

    return transformData;
}

public float4 WorldToClip(float3 positionWS)
{
    return mul(_ViewProjMat, float4(positionWS, 1));
}

public float3 ClipToWorld(float4 positionCS)
{
    float4 worldPos = mul(_InvViewProjMat, positionCS);
    return worldPos.xyz / worldPos.w; 
}
public float3 WorldToView(float3 positionWS)
{
    return mul(_ViewMat, float4(positionWS, 1)).xyz;
}

public float4 ViewToClip(float3 positionVS)
{
    return mul(_ProjMat, float4(positionVS, 1));
}

public float3 ObjToWorld(TransformData transform, float3 positionOS)
{
    return mul(transform.objToWorldMat, float4(positionOS, 1.0)).xyz;
}

public float3 ObjToWorldDir(TransformData transform, float3 dirOS)
{
    return mul((float3x3)transform.objToWorldDirMat, dirOS); 
}

public float3 UVDepthToWorld(float2 uv, float depth)
{
    float2 ndcXY = uv * 2.0 - 1.0; 
    float4 clipPos = float4(ndcXY, depth, 1.0);
    return ClipToWorld(clipPos);
}