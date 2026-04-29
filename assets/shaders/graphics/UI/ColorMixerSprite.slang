#include "Core/BeerMacros.slang"

import Core.BeerCore;
import UI.SpriteMesh;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _CanvasColor;
}

BIND_MATERIAL_TEX(1) Sampler2D _PaintTex;
BIND_MATERIAL_TEX(2) Sampler2DArray _DebugArrayTex;

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
    // float4 debugSample = _DebugArrayTex.Sample(float3(input.uv, 0));
    // return float4(debugSample.rgb, 1);

    float4 paintSample = _PaintTex.Sample(input.uv);
    float3 canvasColor = lerp(_CanvasColor.rgb, paintSample.rgb, paintSample.a);
    return float4(paintSample.rgb, 1.0f);
   // return float4(canvasColor, _CanvasColor.a);
}