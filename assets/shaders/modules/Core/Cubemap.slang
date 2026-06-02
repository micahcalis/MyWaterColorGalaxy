#include "Core/BeerMacros.slang"

module Cubemap;

public enum class CubeFace : uint
{
    Right = 0,
    Left = 1,
    Top = 2,
    Bottom = 3,
    Front = 4,
    Back = 5
}

public float3 GetDirFromCubeFace(float2 uv, CubeFace face) 
{
    float3 dir;

    switch(face) 
    {
        case CubeFace.Right: dir = float3( 1.0, -uv.y, -uv.x); break; 
        case CubeFace.Left: dir = float3(-1.0, -uv.y,  uv.x); break;
        case CubeFace.Top: dir = float3( uv.x,  1.0,  uv.y); break; 
        case CubeFace.Bottom: dir = float3( uv.x, -1.0, -uv.y); break; 
        case CubeFace.Front: dir = float3( uv.x, -uv.y,  1.0); break; 
        case CubeFace.Back: dir = float3(-uv.x, -uv.y, -1.0); break;
    }

    return normalize(dir); 
}