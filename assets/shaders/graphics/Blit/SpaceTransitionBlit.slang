#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Galaxy.HyperSpace;
import Core.Samplers;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _TransitionTime;
    float _EffectScale;
    float4 _LightColor;
    float4 _DarkColor;
    int _FadeState;
    float4 _ColorA;
    float4 _ColorB;
    float4 _ColorC;
    float4 _ColorD;
}

BIND_BLIT_SRC(1)
BIND_MATERIAL_TEX(2) Sampler2D<float> _NoiseTex;

struct MeshData
{
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float2 uv;
}

//$ Pass BlitColor
//$ Vertex SpaceTransitionVert
//$ Fragment SpaceTransitionFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData SpaceTransitionVert(MeshData input, uint vertexID : SV_VertexID)
{
    VertexData output;
    Blit.BlitTriangle blit = Blit.BlitTriangle(vertexID);
    blit.FillData(output.positionCS, output.uv);
    return output;
}
public struct ColorSampler : Galaxy.IColorSampler
{
    private float3[4] colors;

    public __init()
    {
        colors = {
            _ColorA.rgb,
            _ColorB.rgb,
            _ColorC.rgb,
            _ColorD.rgb};
    }

    public float3 GetColor(float hash)
    {
        uint index = uint(hash * 3283) % 4;
        index = clamp(index, 0, 3);
        return colors[index];
    }
}

FRAG float4 SpaceTransitionFrag(VertexData input) : SV_Target
{
    float4 sample = _BlitSource.Sample(input.uv);
    
    bool fadeIn = _FadeState == 0;
    float3 blendCol = 0;

    if (fadeIn)
    {
        Galaxy.HyperSpace<ColorSampler> hyperSpace = Galaxy.HyperSpace<ColorSampler>(input.uv,
            _ScreenParams.x / _ScreenParams.y,
            _EffectScale,
            _LightColor.rgb,
            _DarkColor.rgb,
            ColorSampler());

        float3 hyperSpaceCol = hyperSpace.Resolve(_TransitionTime);
        blendCol = lerp(sample.rgb, hyperSpaceCol, _TransitionTime);
    }
    else
    {
        blendCol = lerp(sample.rgb, _LightColor.rgb, _TransitionTime);
    }

    return float4(blendCol, sample.a);
}