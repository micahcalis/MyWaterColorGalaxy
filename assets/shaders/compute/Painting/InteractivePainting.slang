#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.PerlinNoise;
import Painting.ColorMixing;
import Math.PartialDerivative;
import Painting.PaintBrush;
import Painting.Pigment;
import Painting.FluidPhysics;
import Physics.Flux;
import Painting.KubelkaMunk;

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
    float _TransferSpeed;
    float4 _CanvasColor;
    float _SuspendedOpacity;
    float _EvaporationRate;
}

BIND_MATERIAL_TEX(1) Sampler2D _CanvasSource;
BIND_MATERIAL_TEX(2) RWTexture2D<float> _ShallowWater;
BIND_MATERIAL_TEX(3) RWTexture2DArray<float4> _SuspendedPigmentSource;
BIND_MATERIAL_TEX(4) RWTexture2DArray<float4> _SuspendedPigmentTarget;
BIND_MATERIAL_TEX(5) RWTexture2DArray<float4> _DepositedPigment;
BIND_MATERIAL_TEX(6) RWTexture2D<float4> _PigmentRender;
BIND_MATERIAL_TEX(7) RWTexture2D<float4> _FluxBuffer;

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
        float currentData = _ShallowWater[coord];
        currentData += fluid;
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
    brushPos.y = 1.0f - brushPos.y;

    float2 diff = (threadPos - brushPos);
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
            return _ShallowWater[uint2(baseCoord)];
        }

        return _ShallowWater[uint2(coord)];
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
    FluxReader fluxReader = FluxReader(id.xy);
    Physics.Flux2D currentFlux = fluxReader.Read(0);

    Painting.FluxUpdater<WaterVolumeReader> fluxUpdater = 
        Painting.FluxUpdater<WaterVolumeReader>(volumeReader, currentFlux);

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

        return _SuspendedPigmentSource[uint3(coord, level)];
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
    newPigment = max(newPigment, 1e-6f);
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
    newVolume = max(newVolume, 1e-4);
    _ShallowWater[id.xy] = newVolume;
}

public struct CanvasHeightReader : Painting.IHeightReader
{
    private float2 uv;

    public __init(float2 _uv)
    {
        uv = _uv;
    }

    public float Read()
    {
        return _CanvasSource.SampleLevel(uv, 0.0f).r;
    }
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
    float2 uv = ((float2)id.xy + 0.5) / _PaintResolution;
    CanvasHeightReader heightReader = CanvasHeightReader(uv);
    float4 suspendedWrite;
    float4 depositedWrite;

    Painting.PigmentTransferer<SuspendedPigmentReader,
        DepositedPigmentReader,
        CanvasHeightReader> transferer;

    for (int i = 0; i < 4; i++)
    {
        Painting.Pigment pigment = Painting.GetPigmentFromThread(id.z, i);
        SuspendedPigmentReader suspendedReader = SuspendedPigmentReader(pigment, id.xy);
        DepositedPigmentReader depositedReader = DepositedPigmentReader(pigment, id.xy);

        transferer = Painting.PigmentTransferer<SuspendedPigmentReader,
            DepositedPigmentReader,
            CanvasHeightReader>(suspendedReader, 
                depositedReader, 
                heightReader,
                pigment,
                _TransferSpeed);
        
        Painting.TransferOutput transfer = transferer.Transfer();
        suspendedWrite[i] = transfer.Suspended;
        depositedWrite[i] = transfer.Deposited;
    }

    _SuspendedPigmentTarget[id] = suspendedWrite;
    _DepositedPigment[id] = depositedWrite; 
}

#define PIGMENT_LAYERS 3

public struct ThicknessReader : Painting.IThicknessReader
{
    private float4[PIGMENT_LAYERS] samples;

    public __init(uint2 _coord)
    {
        for (int i = 0; i < PIGMENT_LAYERS; i++)
        {
            samples[i] = _DepositedPigment[uint3(_coord, i)];
            samples[i] += _SuspendedPigmentTarget[uint3(_coord, i)] * _SuspendedOpacity;
        }
    }

    public float Read(int index)
    {
        Painting.PigmentLocation location = Painting.PigmentLocation((Painting.Pigment)index);
        return samples[location.Layer][location.ChannelIndex];
    }
}

//$ Kernel RenderPigment

COMP
[numthreads(8, 8, 1)]
void RenderPigment(uint3 id : SV_DispatchThreadID)
{
    ThicknessReader thicknessReader = ThicknessReader(id.xy);
    Painting.KubelkaMunkRenderer<ThicknessReader> renderer = Painting.KubelkaMunkRenderer<ThicknessReader>(
        thicknessReader,
        _CanvasColor.rgb);

    _PigmentRender[id.xy] = float4(renderer.Render(), 1.0f);
}

//$ Kernel EvaporateWater

COMP
[numthreads(8, 8, 1)]
void EvaporateWater(uint3 id : SV_DispatchThreadID)
{
    WaterVolumeReader waterVolumeReader = WaterVolumeReader(id.xy);

    float currentVolume = waterVolumeReader.Read(0);
    float evaporatedVolume = currentVolume - _EvaporationRate * _DeltaTime * 1e-5f;
    evaporatedVolume = max(evaporatedVolume, 1e-4f);

    _ShallowWater[id.xy] = evaporatedVolume;
}

//$ Kernel ClearLiquids

COMP
[numthreads(8, 8, 1)]
void ClearLiquids(uint3 id : SV_DispatchThreadID)
{
    if (id.z == 0)
    {
        _ShallowWater[id.xy] = 0;
        _FluxBuffer[id.xy] = 0;
    }
    
    _SuspendedPigmentSource[id] = 0;
    _SuspendedPigmentTarget[id] = 0;
    _DepositedPigment[id] = 0;
}
