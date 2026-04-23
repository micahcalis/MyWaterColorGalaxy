#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.PerlinNoise;
import Painting.ColorMixing;
import Math.PartialDerivative;
import Painting.PaintBrush;
import Painting.Pigment;
import Painting.FluidPhysics;
import Physics.Flux;

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
    float _TextureSlopeIntensity;
}

BIND_MATERIAL_TEX(1) Sampler2D _CanvasSource;
BIND_MATERIAL_TEX(2) RWTexture2D<float4> _CanvasTarget;
BIND_MATERIAL_TEX(3) RWTexture2D<float4> _ShallowWater;
BIND_MATERIAL_TEX(4) RWTexture2DArray<float4> _SuspendedPigment;
BIND_MATERIAL_TEX(5) RWTexture2DArray<float4> _DepositedPigment;
BIND_MATERIAL_TEX(6) RWTexture2D<float4> _PigmentRender;
BIND_MATERIAL_TEX(7) RWTexture2D<float4> _FluxBuffer;

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
        float4 currentData = _ShallowWater[coord];
        currentData.b += water;
        _ShallowWater[coord] = saturate(currentData);
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

public struct WaterVolumeReader : Painting.IVolumeReader
{
    private int2 baseCoord;

    public __init(int2 _baseCoord)
    {
        baseCoord = _baseCoord;
    }

    public float Read(int2 offset)
    {
        int2 coord = baseCoord + offset;

        if (any(coord < 0) || coord.x >= _PaintResolution.x || coord.y >= _PaintResolution.y)
        {
            return _ShallowWater[uint2(baseCoord)].b;
        }

        return _ShallowWater[uint2(coord)].b;
    }
}

public struct CanvasSlopeReader : Painting.ITextureSlopeReader
{
    private int2 baseCoord;

    public __init(int2 _baseCoord)
    {
        baseCoord = _baseCoord;
    }

    public Painting.TextureSlope Read(int2 offset)
    {
        uint2 coord = (uint2)(clamp(baseCoord + offset, 0, _PaintResolution - 1));
        float4 sample = _CanvasTarget[coord];

        Painting.TextureSlope slope;
        slope.X = (sample.r - 0.5f) * 2.0f; 
        slope.Y = (sample.g - 0.5f) * 2.0f;
        slope.Height = sample.b;

        return slope;
    }
}

public struct FluxReader : Painting.IFlux2DReader
{
    private int2 baseCoord;

    public __init(int2 _baseCoord)
    {
        baseCoord = _baseCoord;
    }

    public Physics.Flux2D Read(int2 offset)
    {
        int2 coord = baseCoord + offset;

        if (any(coord < 0) || coord.x >= _PaintResolution.x || coord.y >= _PaintResolution.y)
        {
            Physics.Flux2D zeroFlux = (Physics.Flux2D)0; 
            return zeroFlux;
        }

        float4 sample = _FluxBuffer[uint2(coord)];

        Physics.Flux2D flux;
        flux.Left = sample.r; 
        flux.Right = sample.g; 
        flux.Up = sample.b; 
        flux.Down = sample.a; 

        return flux;
    }
}

//$ Kernel CalculateFluidFlux

COMP
[numthreads(8, 8, 1)]
void CalculateFluidFlux(uint3 id : SV_DispatchThreadID)
{
    WaterVolumeReader volumeReader = WaterVolumeReader(id.xy);
    CanvasSlopeReader slopeReader = CanvasSlopeReader(id.xy);
    FluxReader fluxReader = FluxReader(id.xy);
    Physics.Flux2D currentFlux = fluxReader.Read(0);

    Painting.FluxUpdater<WaterVolumeReader, CanvasSlopeReader> fluxUpdater = 
        Painting.FluxUpdater<WaterVolumeReader, CanvasSlopeReader>(volumeReader, slopeReader, currentFlux);

    Physics.Flux2D newFlux = fluxUpdater.UpdateFlux();

    _FluxBuffer[id.xy] = float4(newFlux.Left,
        newFlux.Right,
        newFlux.Up,
        newFlux.Down);
}

COMP
[numthreads(8, 8, 1)]
void ResolveFluidFlux(uint3 id : SV_DispatchThreadID)
{
    FluxReader fluxReader = FluxReader(id.xy);
    WaterVolumeReader volumeReader = WaterVolumeReader(id.xy);
    float currentVolume = volumeReader.Read(0);

    Painting.FluxResolver<FluxReader> fluxResolver =
        Painting.FluxResolver<FluxReader>(fluxReader, currentVolume);

    float newVolume = fluxResolver.Resolve();
    _ShallowWater[id.xy] = float4(0, 0, newVolume, 0);
}