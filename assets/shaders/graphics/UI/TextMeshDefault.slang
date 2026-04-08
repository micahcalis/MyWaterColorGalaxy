#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer2D;
import UI.TextMesh;

//$ Pass UserInterface
//$ Vertex TextVert
//$ Fragment TextFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Text.VertData TextVert(Text.MeshData input)
{
    Text.VertData output;
    output.positionCS = Text.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

FRAG float4 TextFrag(Text.VertData input) : SV_Target
{
    Text.MSDF msdf = Text.MSDF(input.uv);
    return float4(_FontColor.rgb, msdf.GetOpacity() * _FontColor.a);
}