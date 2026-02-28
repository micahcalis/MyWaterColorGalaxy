#include "BeerMacros.slang"

module BeerCore;

cbuffer EngineGlobals : SLOT_ENGINE_GLOBALS
{
    float _Time;
    float _DeltaTime;
    float4x4 _ViewProjMat;
    float4x4 _InvViewProjMat;
    float3 _CameraPos;
    float4 _ScreenParams;
};
