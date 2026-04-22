#include "Core/BeerMacros.slang"

import Core.BeerCore;

module PartialDerivative;

namespace Math
{
    public struct PartialDerivative
    {
        public float X;
        public float Y;

        public __init(float x, float y)
        {
            X = x;
            Y = y;
        }

        [mutating]
        public void Compress()
        {
            X = X * 0.5 + 0.5;
            Y = Y * 0.5 + 0.5;
        }

        [mutating]
        public void Uncompress()
        {
            X = (X - 0.5) * 2.0;
            Y = (Y - 0.5) * 2.0;
        }
    }

    static const float[9] SOBEL_X = float[9](-1, 0, 1, -2, 0, 2, -1, 0, 1);
    static const float[9] SOBEL_Y = float[9](-1, -2, -1, 0, 0, 0, 1, 2, 1);

    public interface IHeightSampler
    {
        public float Sample(float2 uv);
    }

    public struct PartialDerivativeSolver<THeightSampler : IHeightSampler>
    {
        private float2 uv;
        private float2 texelSize;
        private THeightSampler heightSampler;

        public __init(float2 _uv, 
            float2 _texelSize, 
            THeightSampler _heightSampler)
        {
            uv = _uv;
            texelSize = _texelSize;
            heightSampler = _heightSampler;
        }

        public PartialDerivative Solve()
        {
            float resultX = 0;
            float resultY = 0;
            int arrayIndex = -1;

            for (int y = -1; y <= 1; y++)
            {
                for (int x = -1; x <= 1; x++)
                {
                    arrayIndex++;

                    if (x == 0 && y == 0)
                        continue;

                    float2 sampleCoord = uv + float2(x, y) * texelSize;
                    float sample = heightSampler.Sample(sampleCoord);

                    resultX += sample * SOBEL_X[arrayIndex];
                    resultY += sample * SOBEL_Y[arrayIndex];
                }
            }

            return PartialDerivative(resultX / 8.0f, resultY / 8.0f);
        }
    }
}