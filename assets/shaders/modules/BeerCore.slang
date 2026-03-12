#include "BeerMacros.slang"

module BeerCore;

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

BIND_MODEL_PUSH
public cbuffer ModelParams
{
    public float4x4 _ObjToWorldMat;
    public float4x4 _InvObjToWorldMat;
};
