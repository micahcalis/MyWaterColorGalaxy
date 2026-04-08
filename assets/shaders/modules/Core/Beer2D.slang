#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Math.BeerMath;

module Beer2D;

public enum AnchorMode
{
    Center = 0,
    BottomLeft = 1,
    TopLeft = 2,
    BottomRight = 3,
    TopRight = 4
};

BIND_MODEL_PUSH
public cbuffer RectParams
{
    public float2 _RectAnchor;
    public float2 _RectScale;
    public float _RectDepth;
    public float _RectRotation;
    public AnchorMode _RectMode;
};

float2 GetRectPivot(AnchorMode anchorMode)
{
    float2 pivot = float2(0.0, 0.0);
    switch (anchorMode)
    {
        case AnchorMode.Center:
            pivot = float2(0.5, 0.5);
            break;
        case AnchorMode.BottomLeft:
            pivot = float2(0.0, 0.0);
            break;
        case AnchorMode.TopLeft:
            pivot = float2(0.0, 1.0);
            break;
        case AnchorMode.BottomRight:
            pivot = float2(1.0, 0.0);
            break;
        case AnchorMode.TopRight:
            pivot = float2(1.0, 1.0);
            break;
    }

    return pivot;
}

public float4 GetVertexScreenPos2D(uint vertexIndex)
{
    uint localIndex = vertexIndex % 4;
    
    float2 localPos;
    localPos.x = float(localIndex / 2);
    localPos.y = float(localIndex % 2);

    float2 pivot = GetRectPivot(_RectMode);
    localPos -= pivot;

    localPos *= _RectScale;
    localPos = Math.Rotate2D(localPos, _RectRotation);
    localPos += _RectAnchor;

    return float4(localPos.x, localPos.y, _RectDepth, 1);
}