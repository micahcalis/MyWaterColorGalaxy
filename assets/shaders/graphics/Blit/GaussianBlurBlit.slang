#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Blur.GaussianBlurring;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    Blur.BlurDirection _BlurDirection;
    int _BlurResolution;
    float _BlurSpread;
    float2 _SourceTexelSize;
}

BIND_BLIT_SRC(1)

struct MeshData
{
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float2 uv;
}

//$ Pass BlitColor
//$ Vertex GaussianBlurVert
//$ Fragment GaussianBlurFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData GaussianBlurVert(MeshData input, uint vertexID : SV_VertexID)
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

FRAG float4 GaussianBlurFrag(VertexData input) : SV_Target
{
    ColorSampler colorSampler = ColorSampler(input.uv); 

    Blur.GaussianBlurrer<ColorSampler> blurrer = Blur.GaussianBlurrer<ColorSampler>(
        colorSampler,
        _BlurDirection,
        _BlurResolution,
        _BlurSpread,
        _SourceTexelSize);

    return blurrer.GetBlurredColor();
}