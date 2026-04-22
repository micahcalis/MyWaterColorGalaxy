#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.PerlinNoise;
import Painting.ColorMixing;
import Math.PartialDerivative;
import Painting.PaintBrush;
import Painting.Pigment;

BIND_MATERIAL_DATA
cbuffer ComputeData
{
    float2 _RectTopRight;
    float2 _RectBotRight;
    float2 _RectTopLeft;
    float2 _RectBotLeft;
    float2 _MousePos;
    int _MouseClick;
    float2 _PaintResolution;
    float _BrushRadius;
    float _BrushIntensity;
    float _BrushSmoothness;
    Painting.Pigment _BrushPigment;
}

BIND_MATERIAL_TEX(1) Sampler2D _CanvasSource;
BIND_MATERIAL_TEX(2) RWTexture2D<float4> _CanvasTarget;
BIND_MATERIAL_TEX(3) RWTexture2D<float4> _ShallowWater;
BIND_MATERIAL_TEX(4) RWTexture2DArray<float4> _SuspendedPigment;
BIND_MATERIAL_TEX(5) RWTexture2DArray<float4> _DepositedPigment;
BIND_MATERIAL_TEX(6) RWTexture2D<float4> _PigmentRender;

public struct CanvasSampler : Math.IHeightSampler
{
    public float Sample(float2 uv)
    {
        return _CanvasSource.SampleLevel(uv, 0.0f).r;
    }
}

//$ Kernel ComputeCanvas

COMP
[numthreads(8, 8, 1)]
void ComputeCanvas(uint3 id : SV_DispatchThreadID)
{
    float2 uv = ((float2)id.xy + 0.5) / _PaintResolution;
    CanvasSampler canvasSampler = CanvasSampler();

    Math.PartialDerivativeSolver<CanvasSampler> partialDerivSolver = Math.PartialDerivativeSolver<CanvasSampler>(
        uv, 
        float2( 1.0f / _PaintResolution.x, 1.0f / _PaintResolution.y),
        canvasSampler);

    Math.PartialDerivative partialDerivative = partialDerivSolver.Solve();
    partialDerivative.Compress();
    float height = canvasSampler.Sample(uv);

    _CanvasTarget[id.xy] = float4(partialDerivative.X, partialDerivative.Y, height, 0);
}

//$ Kernel InjectPaint

public struct ShallowWaterWrite : Painting.IWaterWrite
{
    private uint2 coord;

    public __init(uint2 _coord)
    {
        coord = _coord;
    }

    public void Write(float water)
    {
        _ShallowWater[coord].a += water;
    }
}

COMP
[numthreads(8, 8, 1)]
void InjectPaint(uint3 id : SV_DispatchThreadID)
{
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

    float2 threadPos = textureCanvas.AlignToCanvas(id.xy);
    float2 brushPos = screenCanvas.AlignToCanvas(_MousePos);

    float2 diff = threadPos - brushPos;
    float distSq = dot(diff, diff);
    float radiusSq = _BrushRadius * _BrushRadius;

    if (distSq > radiusSq)
        return;

    float brushDist = sqrt(distSq);

    Painting.Brush brush = Painting.Brush(_BrushRadius,
        _BrushSmoothness,
        _BrushIntensity,
        _BrushPigment);

    ShallowWaterWrite waterWrite = ShallowWaterWrite(id.xy);
    brush.PaintOnWater(waterWrite, brushDist);

    //TODO: PIGMENT INJECTING
}

