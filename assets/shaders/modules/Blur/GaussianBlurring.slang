#include "Core/BeerMacros.slang"

import Core.BeerCore;

namespace Blur
{
    public interface IColorSampler
    {
        public float4 Sample(float2 offset);
    }

    public enum BlurDirection : int
    {
        Horizontal = 0,
        Vertical = 1
    }

    public struct GaussianBlurrer<TColorSampler : IColorSampler>
    {
        private TColorSampler colorSampler;
        private BlurDirection direction;
        private int blurResolution;
        private float blurSpread;
        private float2 texelSize;

        public __init(TColorSampler _colorSampler,
            BlurDirection _direction,
            int _blurResolution,
            float _blurSpread,
            float2 _texelSize)
        {
            colorSampler = _colorSampler;
            direction = _direction;
            blurResolution = _blurResolution;
            blurSpread = _blurSpread;
            texelSize = _texelSize;
        }

        public float4 GetBlurredColor()
        {
            float4 colorSum = (float4)0;
            float weightSum = 1e-5f;
            float2 offset = direction == BlurDirection::Horizontal 
                ? float2(texelSize.x, 0) : float2(0, texelSize.y);

            for (int i = -blurResolution; i <= blurResolution; i++)
            {
                float gaussWeight = GaussWeight(i, blurSpread);
                float4 sample = colorSampler.Sample(offset * (float)i);

                colorSum += sample * gaussWeight;
                weightSum += gaussWeight;
            }

            return colorSum / weightSum;
        }

        public static float GaussWeight(float offset, float spread)
        {
            float x = offset;
            float sigmaSqu = spread * spread;
	        return (1.0f / sqrt(2.0f * PI * sigmaSqu)) * exp(-(x * x) / (2 * sigmaSqu));
        }
    }
}