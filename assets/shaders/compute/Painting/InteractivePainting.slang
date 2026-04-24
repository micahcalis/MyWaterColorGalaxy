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
    float _TransferSpeed;
}

BIND_MATERIAL_TEX(1) Sampler2D _CanvasSource;
BIND_MATERIAL_TEX(2) RWTexture2D<float4> _CanvasTarget;
BIND_MATERIAL_TEX(3) RWTexture2D<float4> _ShallowWater;
BIND_MATERIAL_TEX(4) RWTexture2DArray<float4> _SuspendedPigmentSource;
BIND_MATERIAL_TEX(5) RWTexture2DArray<float4> _SuspendedPigmentTarget;
BIND_MATERIAL_TEX(6) RWTexture2DArray<float4> _DepositedPigment;
BIND_MATERIAL_TEX(7) RWTexture2D<float4> _PigmentRender;
BIND_MATERIAL_TEX(8) RWTexture2D<float4> _FluxBuffer;

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

public struct ShallowWaterWrite : Painting.IFluidWrite
{
    private uint2 coord;

    public __init(uint2 _coord)
    {
        coord = _coord;
    }

    public void Write(float fluid)
    {
        float4 currentData = _ShallowWater[coord];
        currentData.b += fluid;
        _ShallowWater[coord] = currentData;
    }
}

public struct PigmentChannelWrite : Painting.IFluidWrite
{
    private uint2 coord;
    private Painting.PigmentLocation location;

    public __init(uint2 _coord, Painting.Pigment _pigment)
    {
        coord = _coord;
        location = Painting.PigmentLocation(_pigment);
    }

    public void Write(float fluid)
    {
        uint3 layerCoord = uint3(coord, location.Layer);
        float4 currentData = _SuspendedPigmentSource[layerCoord];
        currentData[location.ChannelIndex] += fluid;
        _SuspendedPigmentSource[layerCoord] = currentData;
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
    PigmentChannelWrite pigmentWrite = PigmentChannelWrite(id.xy, _BrushPigment);
    brush.Paint(waterWrite, pigmentWrite, brushDist);
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
        Painting.FluxUpdater<WaterVolumeReader, CanvasSlopeReader>(volumeReader, slopeReader, currentFlux, _TextureSlopeIntensity);

    Physics.Flux2D newFlux = fluxUpdater.UpdateFlux();

    _FluxBuffer[id.xy] = float4(newFlux.Left,
        newFlux.Right,
        newFlux.Up,
        newFlux.Down);
}

public struct SuspendedReader : Painting.ISuspendedReader
{
    private int2 baseCoord;
    private int level;

    public __init(int2 _baseCoord, int _level)
    {
        baseCoord = _baseCoord;
        level = _level;
    }

    public float4 Read(int2 offset)
    {
        int2 coord = baseCoord + offset;

        if (any(coord < 0) || coord.x >= _PaintResolution.x || coord.y >= _PaintResolution.y)
        {
            return float4(0.0f);
        }

        return _SuspendedPigmentSource[uint3(baseCoord, level)];
    }
}

//$ Kernel ResolvePigmentFlux

COMP
[numthreads(8, 8, 1)]
void ResolvePigmentFlux(uint3 id : SV_DispatchThreadID)
{
    FluxReader fluxReader = FluxReader(id.xy);
    WaterVolumeReader volumeReader = WaterVolumeReader(id.xy);
    SuspendedReader suspendedReader = SuspendedReader(id.xy, id.z);
    float4 currentPigment = suspendedReader.Read(0);

    Painting.PigmentResolver<WaterVolumeReader, FluxReader, SuspendedReader> pigmentResolver =
        Painting.PigmentResolver<WaterVolumeReader, FluxReader, SuspendedReader>(volumeReader, 
            fluxReader, 
            suspendedReader, 
            currentPigment);
    
    float4 newPigment = pigmentResolver.Resolve();
    _SuspendedPigmentTarget[id] = newPigment;
}

//$ Kernel ResolveWaterFlux

COMP
[numthreads(8, 8, 1)]
void ResolveWaterFlux(uint3 id : SV_DispatchThreadID)
{
    FluxReader fluxReader = FluxReader(id.xy);
    WaterVolumeReader volumeReader = WaterVolumeReader(id.xy);
    float currentVolume = volumeReader.Read(0);

    Painting.FluxWaterResolver<FluxReader> fluxResolver =
        Painting.FluxWaterResolver<FluxReader>(fluxReader, currentVolume);

    float newVolume = fluxResolver.Resolve();
    _ShallowWater[id.xy] = float4(0, 0, newVolume, 0);
}

public struct SuspendedPigmentReader : Painting.IPigmentReader
{
    private Painting.PigmentLocation location;
    private uint2 coord;

    public __init(Painting.Pigment _pigment, uint2 _coord)
    {
        location = Painting.PigmentLocation(_pigment);
        coord = _coord;
    }

    public float Read()
    {
        float4 sample = _SuspendedPigmentTarget[uint3(coord, location.Layer)];
        return sample[location.ChannelIndex];
    }
}

public struct DepositedPigmentReader : Painting.IPigmentReader
{
    private Painting.PigmentLocation location;
    private uint2 coord;

    public __init(Painting.Pigment _pigment, uint2 _coord)
    {
        location = Painting.PigmentLocation(_pigment);
        coord = _coord;
    }

    public float Read()
    {
        float4 sample = _DepositedPigment[uint3(coord, location.Layer)];
        return sample[location.ChannelIndex];
    }
}

//$ Kernel TransferPigment

COMP
[numthreads(8, 8, 1)]
void TransferPigment(uint3 id : SV_DispatchThreadID)
{
    CanvasSlopeReader slopeReader = CanvasSlopeReader(id.xy);
    float4 suspendedWrite;
    float4 depositedWrite;

    Painting.PigmentTransferer<SuspendedPigmentReader,
        DepositedPigmentReader,
        CanvasSlopeReader> transferer;

    for (int i = 0; i < 4; i++)
    {
        Painting.Pigment pigment = Painting.GetPigmentFromThread(id.z, i);
        SuspendedPigmentReader suspendedReader = SuspendedPigmentReader(pigment, id.xy);
        DepositedPigmentReader depositedReader = DepositedPigmentReader(pigment, id.xy);

        transferer = Painting.PigmentTransferer<SuspendedPigmentReader,
        DepositedPigmentReader,

        CanvasSlopeReader>(suspendedReader, 
            depositedReader, 
            slopeReader,
            pigment,
            _TransferSpeed);
        
        Painting.TransferOutput transfer = transferer.Transfer();
        suspendedWrite[i] = transfer.Suspended;
        depositedWrite[i] = transfer.Deposited;
    }

    _SuspendedPigmentTarget[id] = suspendedWrite;
    _DepositedPigment[id] = depositedWrite; 
}