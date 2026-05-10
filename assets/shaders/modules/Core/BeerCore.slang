#include "Core/BeerMacros.slang"

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
    public float3 _CameraDir;
    public float4 _ScreenParams;
    public float4 _ZBufferParams;
    public float4x4 _ViewMat;
    public float4x4 _ViewDirMat;
    public float4x4 _ProjMat;
    public float4x4 _UIOrthoProjMat;
};

BIND_LIGHTING_GLOBALS
public cbuffer LightingGlobals
{
    public float4 _MainLightColor;
    public float4 _ShadowColor;
    public float4 _SkyColor;
    public float3 _MainLightPos;
}

BIND_GALAXY_GLOBALS
public cbuffer GalaxyGlobals
{
    public float2 _MapBotLeft;
    public float2 _MapBotRight;
    public float2 _MapTopLeft;
    public float2 _MapTopRight;
    public float _MapZoomScale;
    public float2 _MapZoomPanning;
}

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


public struct CameraData
{
    public float3 position;
    public float3 direction;
    public float nearField;
    public float farField;
};

public CameraData GetMainCamera()
{
    CameraData data;
    data.position = _CameraPos;
    data.direction = _CameraDir;
    data.nearField = _ZBufferParams.x;
    data.farField = _ZBufferParams.y;
    return data;
}

public float LinearizeDepth(float hardwareDepth)
{
    float near = _ZBufferParams.x;
    float far = _ZBufferParams.y;

    return (near * far) / (far + hardwareDepth * (near - far));
}

public float GetAspect()
{
    return _ScreenParams.x / _ScreenParams.y;
}

public void ClipInMap(float2 pixelPos)
{
    if (any(pixelPos < _MapBotLeft)
        || any(pixelPos > _MapTopRight))
    {
        discard;
    }
}