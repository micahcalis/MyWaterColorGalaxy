#include "Core/BeerMacros.slang"

import Core.BeerCore;
import UI.SpriteMesh;
import Core.Beer2D;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _TintColor;
}

BIND_MATERIAL_TEX(1) Sampler2D _SpriteTex;

//$ Pass UserInterface
//$ Vertex PlayerIndicatorCompVert
//$ Fragment PlayerIndicatorCompFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Sprite.VertData PlayerIndicatorCompVert(Sprite.MeshData input)
{
    Sprite.VertData output;

    ApplyMapZoom(input.position2DOS);

    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

FRAG float4 PlayerIndicatorCompFrag(Sprite.VertData input) : SV_Target
{
    ClipInMap(input.positionCS.xy);

    float4 sample = _SpriteTex.Sample(input.uv);
    return sample * _TintColor;
}