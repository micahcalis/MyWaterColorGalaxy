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
    float _PaintIntensity;
}

BIND_MATERIAL_TEX(1) RWTexture2D<float4> _PaintTex;
BIND_MATERIAL_TEX(2) RWTexture2DArray<float4> _TestArray;

//$ Kernel PaintMouse

COMP
[numthreads(8, 8, 1)]
void PaintMouse(uint3 id : SV_DispatchThreadID)
{
    _TestArray[uint3(id.xy, 0)] = float4(1, 0, 0, 1);
    bool isClicking = _MouseClick != 0;

    if (!isClicking)
        return;

    Painting.Canvas screenCanvas = Painting.Canvas(_RectTopRight,
        _RectTopLeft,
        _RectBotRight,
        _RectBotLeft);

    Painting.Canvas textureCanvas = Painting.Canvas(_PaintResolution,
        float2(0, _PaintResolution.y),
        float2(_PaintResolution.x, 0),
        float2(0, 0));

    if (!screenCanvas.InCanvas(_MousePos))
        return;

    static float testRadius = 0.1f;

    float2 threadPos = textureCanvas.AlignToCanvas(id.xy);
    float2 brushPos = screenCanvas.AlignToCanvas(_MousePos);

    float brushDist = distance(threadPos, brushPos);

    if (brushDist > testRadius)
        return;

    float paintVal = saturate(1 - (brushDist / testRadius));
    float paintDelta = _DeltaTime * _PaintIntensity;
    float mixAlpha = paintVal * paintDelta;

    float4 sample = _PaintTex[id.xy];

    Painting.Mixer mixer = Painting.Mixer(sample.rgb, sample.a);
    mixer.Mix(_PaintColor.rgb, mixAlpha);

    _PaintTex[id.xy] = float4(mixer.GetColor(), mixer.GetAlpha());
}