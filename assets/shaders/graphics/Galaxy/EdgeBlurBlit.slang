#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Galaxy.EdgeBlur;
import Blur.GaussianBlurring;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    Blur.BlurDirection _BlurDirection;
    int _BlurResolution;
    float _BlurSpread;
    float _DepthThreshold;
}

BIND_BLIT_SRC(1)
BIND_MATERIAL_TEX(2) Sampler2D<float> _DepthBuffer;

struct MeshData
{
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float2 uv;
}

//$ Pass BlitColor
//$ Vertex EdgeBlurVert
//$ Fragment EdgeBlurFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData EdgeBlurVert(MeshData input, uint vertexID : SV_VertexID)
{
    VertexData output;
    Blit.BlitTriangle blit = Blit.BlitTriangle(vertexID);
    blit.FillData(output.positionCS, output.uv);
    return output;
}

public struct ColorSampler : Blur.IColorSampler
{
    private float2 uv;

    public __init(float2 _uv)
    {
        uv = _uv;
    }

    public float4 Sample(float2 offset)
    {
        return _BlitSource.Sample(saturate(uv + offset));
    }
}

public struct DepthSampler : Blur.IDepthSampler
{
    private float2 uv;

    public __init(float2 _uv)
    {
        uv = _uv;
    }

    public float SampleLinear(float2 offset)
    {
        return GetLinearDepth(_DepthBuffer.Sample(saturate(uv + offset)));
    }
}
    
FRAG float4 EdgeBlurFrag(VertexData input) : SV_Target
{
    ColorSampler colorSampler = ColorSampler(input.uv);
    DepthSampler depthSampler = DepthSampler(input.uv);

    Blur.EdgeBlurrer<ColorSampler, DepthSampler> blurrer = Blur.EdgeBlurrer<ColorSampler, DepthSampler>(
        colorSampler, 
        _BlurDirection,
        _BlurResolution,
        _BlurSpread,
        rcp(_ScreenParams.xy),
        depthSampler,
        _DepthThreshold);

    float4 blurredColor = blurrer.GetBlurredColor();
    float4 baseSample = colorSampler.Sample(0);

    return float4(baseSample.rg, blurredColor.ba);
}