#include "Core/BeerMacros.slang"

import UI.SpriteMesh;
import UI.GalaxyMap;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    int _StarSeed;
    float4 _StarColor;
    float _StarSize;
    float _GradientIntensity;
    float _EdgeRingIntensity;
    int _GradientSteps;
}

BIND_MATERIAL_TEX(1) Sampler2D _NoiseTexture;

//$ Pass UserInterface
//$ Vertex SunVert
//$ Fragment SunFrag
//$ Cull None
//$ Blend On
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT Sprite.VertData SunVert(Sprite.MeshData input)
{
    Sprite.VertData output;
    output.positionCS = Sprite.GetPositionCS(input);
    output.uv = input.texCoord;
    return output;
}

public struct NoiseSampler : Map.IStarDustNoiseSampler
{
    public float GetNoise(float2 uv)
    {
        return _NoiseTexture.Sample(uv).r;
    }
}

FRAG float4 SunFrag(Sprite.VertData input) : SV_Target
{
    Map.Sun<NoiseSampler> sun = Map.Sun<NoiseSampler>(NoiseSampler(),
        input.uv,
        _StarSeed,
        _StarColor.rgb,
        _StarSize,
        _GradientIntensity,
        _EdgeRingIntensity,
        _GradientSteps);

    return sun.Get();
}