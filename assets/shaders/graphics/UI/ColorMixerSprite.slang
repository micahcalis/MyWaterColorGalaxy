#include "Core/BeerMacros.slang"

import Core.BeerCore;
import UI.SpriteMesh;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _CanvasColor;
}

BIND_MATERIAL_TEX(1) Sampler2D _PaintTex;
BIND_MATERIAL_TEX(2) Sampler2D _SpriteTex;

//$ Pass UserInterface
//$ Vertex ColorMixerVert
//$ Fragment ColorMixerFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Sprite.VertData ColorMixerVert(Sprite.MeshData input)
{
    Sprite.VertData output;
    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

FRAG float4 ColorMixerFrag(Sprite.VertData input) : SV_Target
{
    float4 paintSample = _PaintTex.Sample(input.uv);
    float4 spriteSample = _SpriteTex.Sample(input.uv);
    return float4(paintSample.rgb * spriteSample.rgb, spriteSample.a);
}