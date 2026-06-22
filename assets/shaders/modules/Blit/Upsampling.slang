#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Samplers;

namespace Blit
{
    enum class UpsampleDirection : uint
    {
        BottomLeft = 0,
        BottomRight = 1,
        TopLeft = 2,
        TopRight = 3,
        Center = 4
    }

    static const int2[5] UPSAMPLE_DIR = {
        int2(-1, -1),
        int2(1, -1),
        int2(-1, 1),
        int2(1, 1),
        int2(0, 0)
    };

    inline int2 GetOffset(UpsampleDirection dir)
    {
        return UPSAMPLE_DIR[(uint)dir];
    }

    public struct Upsampler<TDepthSampler : ISampler2DFloat>
    {
        private TDepthSampler depthSampler;
        private float depthTolerance;
        private float2 uv;
        private float2 texelSize;
        float[4] depthWeights = (float[4])0;

        public __init(TDepthSampler _depthSampler,
            float _depthTolerance,
            float2 _uv,
            float2 _texelSize)
        {
            depthSampler = _depthSampler;
            depthTolerance = _depthTolerance;
            uv = _uv;
            texelSize = _texelSize;
        }

        [mutating]
        public void ComputeWeights()
        {
            float fullDepth = depthSampler.Sample(uv);

            depthWeights[UpsampleDirection::BottomLeft] = GetWeight(fullDepth, UpsampleDirection::BottomLeft);
            depthWeights[UpsampleDirection::BottomRight] = GetWeight(fullDepth, UpsampleDirection::BottomRight);
            depthWeights[UpsampleDirection::TopLeft] = GetWeight(fullDepth, UpsampleDirection::TopLeft);
            depthWeights[UpsampleDirection::TopRight] = GetWeight(fullDepth, UpsampleDirection::TopRight);

            float totalWeight =  depthWeights[0] + depthWeights[1] + depthWeights[2]  + depthWeights[3];
            depthWeights[UpsampleDirection::BottomLeft] /= totalWeight;
            depthWeights[UpsampleDirection::BottomRight] /= totalWeight;
            depthWeights[UpsampleDirection::TopLeft] /= totalWeight;
            depthWeights[UpsampleDirection::TopRight] /= totalWeight;
        }

        private inline float GetWeight(float fullDepth, UpsampleDirection dir)
        {
            float depth = depthSampler.Sample(uv + GetOffset(dir) * texelSize);
            return 1.0f / (abs(fullDepth - depth) + depthTolerance);
        }

        public float4 Upsample<TColorSampler : ISampler2DFloat4>(TColorSampler colorSampler)
        {
            float4 totalColor = 0;

            [unroll(4)]
            for (int i = 0; i < 4; i++)
            {
                totalColor += GetWeightedColor(colorSampler, (UpsampleDirection)i);
            }

            return totalColor;
        }

        inline private float4 GetWeightedColor<TColorSampler : ISampler2DFloat4>(TColorSampler colorSampler,
            UpsampleDirection dir)
        {
            return colorSampler.Sample(uv + GetOffset(dir) * texelSize) * depthWeights[dir];
        }
    }
}