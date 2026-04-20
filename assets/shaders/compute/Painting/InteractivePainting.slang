#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.PerlinNoise;
import Painting.ColorMixing;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float2 _RectTopRight;
    float2 _RectBotRight;
    float2 _RectTopLeft;
    float2 _RectBotLeft;
    float2 _MousePos;
    int _MouseClick;
    float _PaintRadius;
    float4 _PaintColor;
    float2 _PaintResolution;
}

BIND_MATERIAL_TEX(1) RWTexture2D<float4> _PaintTex;

//$ Kernel PaintMouse

COMP
[numthreads(8, 8, 1)]
void PaintMouse(uint3 id : SV_DispatchThreadID)
{
    float4 currentVal = _PaintTex[id.xy];
    currentVal -= _DeltaTime * 0.5f;
    _PaintTex[id.xy] = saturate(currentVal);

    bool isClicking = _MouseClick != 0;

    if (!isClicking)
        return;

    Painting.Canvas canvas = Painting.Canvas(_RectTopRight,
        _RectTopLeft,
        _RectBotRight,
        _RectBotLeft);

    if (!canvas.InCanvas(_MousePos))
        return;

    static float testRadius = 10.0f;

    float2 threadPixelPos = _RectBotLeft + id.xy;
    float pixelDist = distance(threadPixelPos, _MousePos);

    if (pixelDist > testRadius)
        return;

    float paintVal = saturate(1 - (pixelDist / testRadius));
    _PaintTex[id.xy] += paintVal;
}