#include "Core/BeerMacros.slang"

import UI.SpriteMesh;
import Core.BeerCore;
import Core.Beer2D;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _PrimaryColor;
    float4 _SecondaryColor;
    float4 _TertiaryColor;
    float4 _QuaternaryColor;
    int _OverrideMapClip;
}

BIND_MATERIAL_TEX(1) Sampler2D _SpriteTex;
BIND_MATERIAL_TEX(2) Sampler2D _ColorMask;

//$ Pass UserInterface
//$ Vertex GalaxyCompVert
//$ Fragment GalaxyCompFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Sprite.VertData GalaxyCompVert(Sprite.MeshData input)
{
    Sprite.VertData output;

    if (_OverrideMapClip == 0)
    {
        ApplyMapZoom(input.position2DOS);
    }

    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

float4 GetMaskColor(float2 uv)
{
    float4 sample = _ColorMask.Sample(uv);
    float4 color = _PrimaryColor;
    color = lerp(color, _PrimaryColor, sample.r);
    color = lerp(color, _SecondaryColor, sample.g);
    color = lerp(color, _TertiaryColor, sample.b);
    return lerp(color, _QuaternaryColor, sample.a);
}

FRAG float4 GalaxyCompFrag(Sprite.VertData input) : SV_Target
{
    if (_OverrideMapClip == 0)
    {
        ClipInMap(input.positionCS.xy);
    }

    float4 sprite = _SpriteTex.Sample(input.uv);
    float4 gradCol = GetMaskColor(input.uv);

    return sprite * gradCol;
}