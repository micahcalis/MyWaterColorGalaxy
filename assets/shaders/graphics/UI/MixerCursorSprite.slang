#include "Core/BeerMacros.slang"

import Core.BeerCore;
import UI.SpriteMesh;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _ActiveColor;
    int _ActiveState;
}

BIND_MATERIAL_TEX(1) Sampler2D _SpriteTex;
BIND_MATERIAL_TEX(2) Sampler2D _MaskTex;

//$ Pass UserInterface
//$ Vertex MixerCursorVert
//$ Fragment MixerCursorFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Sprite.VertData MixerCursorVert(Sprite.MeshData input)
{
    Sprite.VertData output;
    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

FRAG float4 MixerCursorFrag(Sprite.VertData input) : SV_Target
{
    bool isActive = _ActiveState > 0;

    if (!isActive)
        discard;

    float4 spriteSample = _SpriteTex.Sample(input.uv);
    float4 maskSample = _MaskTex.Sample(input.uv);

    float3 rgb = lerp(spriteSample.rgb, _ActiveColor.rgb, maskSample.r);
    return float4(rgb, spriteSample.a);
}