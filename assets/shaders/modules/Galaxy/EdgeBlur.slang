#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Blur.GaussianBlurring;

namespace Blur
{
    public interface IDepthSampler
    {
        public float SampleLinear(float2 offset);
    }

    public struct EdgeBlurrer<TColorSampler : IColorSampler, TDepthSampler : IDepthSampler>
    {
        private TColorSampler colorSampler;
        private BlurDirection direction;
        private int blurResolution;
        private float blurSpread;
        private float2 texelSize;

        private TDepthSampler depthSampler;
        private float depthThreshold;

        public __init(TColorSampler _colorSampler,
            BlurDirection _direction,
            int _blurResolution,
            float _blurSpread,
            float2 _texelSize,
            TDepthSampler _depthSampler,
            float _depthTreshold)
        {
            colorSampler = _colorSampler;
            direction = _direction;
            blurResolution = _blurResolution;
            blurSpread = _blurSpread;
            texelSize = _texelSize;

            depthSampler = _depthSampler;
            depthThreshold = _depthTreshold;
        }
        public float4 GetBlurredColor()
        {
            // 1. Calculate the proper Gaussian weight for the center pixel
            float centerGaussWeight = Functions.GaussWeight(0, blurSpread);

            // 2. Initialize with the weighted center pixel
            float4 colorSum = colorSampler.Sample(0) * centerGaussWeight;
            float weightSum = centerGaussWeight;
            
            float2 offset = direction == BlurDirection::Horizontal 
                ? float2(texelSize.x, 0) : float2(0, texelSize.y);

            float baseDepth = depthSampler.SampleLinear(0);

            for (int i = -blurResolution; i <= blurResolution; i++)
            {
                if (i == 0)
                    continue;

                float gaussWeight = Functions.GaussWeight(i, blurSpread);
                float4 colorSample = colorSampler.Sample(offset * (float)i);

                float depthSample = depthSampler.SampleLinear(offset * (float)i);
                
                // At edges (high diff), depthWeight -> 1 (Normal Blur)
                // On flats (low diff), depthWeight -> 0 (No Blur from neighbors)
                float depthWeight = smoothstep(0, depthThreshold, abs(baseDepth - depthSample));

                float weight = gaussWeight * depthWeight;

                colorSum += colorSample * weight;
                weightSum += weight;
            }

            return colorSum / weightSum;    
        }
    }
}
