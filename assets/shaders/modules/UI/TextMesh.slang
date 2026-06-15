#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer2D;
import Math.BeerMath;

module TextMesh;

BIND_MATERIAL_DATA 
public cbuffer FontData
{
    public float _FontSize;
    public float4 _FontColor;
}

BIND_MATERIAL_TEX(1) Sampler2D _FontAtlas;

namespace Text
{
    public struct MeshData
    {
        public float2 position2DOS;
        public float2 texCoord;
    }

    public struct VertData
    {
        public float4 positionCS : SV_Position;
        public float2 uv;
    }

    public float4 GetPositionCS(MeshData input)
    {
        return float4(input.position2DOS.xy, _RectDepth, 1);
    }

    public struct MSDF
    {        
        private float2 uv;
        private Sampler2D atlas;

        public __init(float2 _uv)
        {
            uv = _uv;
            atlas = _FontAtlas;
        }

        public float GetOpacity()
        {
            float3 msd = atlas.Sample(uv).rgb;
            float sd = Math.Median(msd.r, msd.g, msd.b);
            float screenPxDistance = (sd - 0.5) / max(fwidth(sd), 0.0001);
    
            return clamp(screenPxDistance + 0.5, 0.0, 1.0);
        }
    };
}
