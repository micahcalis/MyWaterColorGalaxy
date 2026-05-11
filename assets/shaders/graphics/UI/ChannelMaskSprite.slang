#include "Core/BeerMacros.slang"

import Core.BeerCore;
import UI.SpriteMesh;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _ColorR;
    float4 _ColorG;
    float4 _ColorB;
    float4 _ColorA;
}

BIND_MATERIAL_TEX(1) Sampler2D _SpriteTex;

//$ Pass UserInterface
//$ Vertex ChannelMaskVert
//$ Fragment ChannelMaskFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Sprite.VertData ChannelMaskVert(Sprite.MeshData input)
{
    Sprite.VertData output;
    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

FRAG float4 ChannelMaskFrag(Sprite.VertData input) : SV_Target
{
    float4 sample = _SpriteTex.Sample(input.uv);
    float alpha = max(sample.r, max(sample.g, max(sample.b, sample.a)));

    float3 rgb = _ColorA.rgb; 
    rgb = lerp(rgb, _ColorB.rgb, step(0.01f, sample.b)); 
    rgb = lerp(rgb, _ColorG.rgb, step(0.01f, sample.g)); 
    rgb = lerp(rgb, _ColorR.rgb, step(0.01f, sample.r)); 
    
    return float4(rgb, alpha);
}