#include "Core/BeerMacros.slang"

import Core.BeerCore;
import UI.SpriteMesh;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    int _GalaxySeed;
    float4 _ColorA;
    float4 _ColorB;
    float4 _ColorC;
}

BIND_MATERIAL_TEX(1) Sampler2D _NoiseTexture;

//$ Pass UserInterface
//$ Vertex GalaxyMapVert
//$ Fragment GalaxyMapFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Sprite.VertData GalaxyMapVert(Sprite.MeshData input)
{
    Sprite.VertData output;
    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

FRAG float4 GalaxyMapFrag(Sprite.VertData input) : SV_Target
{
    return _NoiseTexture.Sample(input.uv);
}