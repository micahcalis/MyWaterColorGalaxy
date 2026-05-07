#include "Core/BeerMacros.slang"

import UI.SpriteMesh;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _PrimaryColor;
    float4 _SecondaryColor;
    float4 _TertiaryColor;
    float4 _QuaternaryColor;
}

BIND_MATERIAL_TEX(1) Sampler2D _SpriteTex;

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
    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

float4 Get4ColorGradient(float gradient)
{
    float scaledT = saturate(gradient) * 3.0f;
    float4 color = lerp(_PrimaryColor, _SecondaryColor, saturate(scaledT));
    color = lerp(color, _TertiaryColor, saturate(scaledT - 1.0f));
    color = lerp(color, _QuaternaryColor, saturate(scaledT - 2.0f));

    return color;
}

FRAG float4 GalaxyCompFrag(Sprite.VertData input)
{
    float gradient = _SpriteTex.Sample(input.uv).r;
    return Get4ColorGradient(1.0 - gradient);
}