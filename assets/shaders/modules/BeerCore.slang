#include "BeerMacros.slang"

module BeerCore;

import Lights;

BIND_ENGINE_GLOBALS
public cbuffer EngineGlobals
{
    public float _Time;
    public float _DeltaTime;
    public float4x4 _ViewProjMat;
    public float4x4 _InvViewProjMat;
    public float3 _CameraPos;
    public float4 _ScreenParams;
};

BIND_LIGHTING_GLOBALS
public cbuffer LightingGlobals
{
    public float4 _MainLightColor;
    public float4 _ShadowColor;
    public float4 _SkyColor;
    public float3 _MainLightPos;
}

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

public float3 GetViewDirectionWS(float3 positionWS)
{
    return normalize(positionWS - _CameraPos);
}

public Light GetMainLight(float3 positionWS)
{
    return Light(_MainLightPos, 
        positionWS, 
        _MainLightColor.rgb, 
        _ShadowColor.rgb, 
        _SkyColor.rgb);
}

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
