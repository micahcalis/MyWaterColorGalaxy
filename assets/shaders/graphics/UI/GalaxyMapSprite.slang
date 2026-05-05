#include "Core/BeerMacros.slang"

import UI.SpriteMesh;
import UI.GalaxyMap;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    int _GalaxySeed;
    float4 _ColorA;
    float4 _ColorB;
    float4 _ColorC;
    float _StarFrequency;
    float _StarScale;
    float4 _StarColor;
    float _DustFrequency;
    float _DustThreshold;
    float _DustEdgeThickness;
    int _DustGradientSteps;
    float _DustOpacity;
    float2 _GalaxyCenter;
    int _RingCount;
    float _RingThickness;
    float4 _RingColor;
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

public struct NoiseSampler : Map.IStarDustNoiseSampler
{
    public float GetNoise(float2 uv)
    {
        return _NoiseTexture.Sample(uv).r;
    }
}

FRAG float4 GalaxyMapFrag(Sprite.VertData input) : SV_Target
{
    Map.Stars starsBig = Map.Stars(_GalaxySeed,
        input.uv,
        _StarFrequency,
        _StarScale,
        _StarColor);

    Map.Stars starsSmall = Map.Stars(_GalaxySeed + 1,
    input.uv,
    _StarFrequency * 2.0f,
    _StarScale,
    _StarColor);

    float3[3] dustColors = {
        _ColorA.rgb,
        _ColorB.rgb,
        _ColorC.rgb
    };

    Map.StarDust<NoiseSampler> starDust = Map.StarDust<NoiseSampler>(NoiseSampler(),
        input.uv,
        _GalaxySeed,
        _DustFrequency,
        dustColors,
        _DustThreshold,
        _DustEdgeThickness,
        _DustGradientSteps,
        _DustOpacity);

    Map.Rings rings = Map.Rings(float2(_GalaxyCenter.x, 1.0f - _GalaxyCenter.y),
        input.uv,
        _GalaxySeed,
        _RingCount,
        _RingThickness,
        _RingColor.rgb);

    float4 starsVal = starsBig.Get() + starsSmall.Get();
    float4 starDustVal = starDust.Get();
    float4 ringsVal = rings.Get();
    
    float3 blendCol =  starsVal.rgb * 10 + starDustVal.rgb;
    blendCol = lerp(blendCol, ringsVal.rgb, ringsVal.a);

    return float4(blendCol, 1);
}