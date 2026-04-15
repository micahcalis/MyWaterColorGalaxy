#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer2D;
import UI.TextMesh;
import UI.SpriteMesh;

//$ Pass UserInterface
//$ Vertex TextVert
//$ Fragment TextFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Text.VertData TextVert(Sprite.MeshData input)
{
    Text.VertData output;
    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

FRAG float4 TextFrag(Text.VertData input) : SV_Target
{
    Text.MSDF msdf = Text.MSDF(input.uv);
    return float4(_FontColor.rgb, msdf.GetOpacity() * _FontColor.a);
}