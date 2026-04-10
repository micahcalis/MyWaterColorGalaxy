#include "Core/BeerMacros.slang"

import Core.BeerCore;
import UI.SpriteMesh;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _TintColor;
    float2 _Scale;
    float2 _Offset;
};

BIND_MATERIAL_TEX(1) Sampler2D _SpriteTex;

//$ Pass UserInterface
//$ Vertex SpriteVert
//$ Fragment SpriteFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Sprite.VertData SpriteVert(Sprite.MeshData input)
{
    Sprite.VertData output;
    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

FRAG float4 SpriteFrag(Sprite.VertData input) : SV_Target
{
    Sprite.Sprite sprite = Sprite.Sprite(input.uv, _SpriteTex);
    sprite.SetScaleOffset(_Scale, _Offset);
    sprite.SetTint(_TintColor);

    return float4(input.uv, 0, 0.5);

    return sprite.GetColor();
}