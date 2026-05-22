#include "Core/BeerMacros.slang"

module FractalScale;

namespace Noise
{
    struct Frequency
    {
        float Max;
        float Min;
    }

    public struct FractalData
    {
        public float LowLevel;
        public float HighLevel;
        public float SubLayer;
    }

    public struct FractalSolver
    {
        private float3 uvw;
        private float fractalScale;

        public __init(float3 _uvw,
            float _fractalScale)
        {
            uvw = _uvw;
            fractalScale = _fractalScale;
        }

        public FractalData Solve()
        {
            Frequency frequency = GetFrequency();

            float spacing = frequency.Min;
            spacing *= exp2(fractalScale);
            float logDepth = log2(spacing);
            float fractalStep = floor(logDepth);

            FractalData data;
            data.LowLevel = exp2(fractalStep);
            data.HighLevel = exp2(fractalStep + 1);
            data.SubLayer = logDepth - fractalStep;
            return data;
        }

        private Frequency GetFrequency()
        {
            Frequency frequency;

            float3 dx = ddx(uvw);
            float3 dy = ddy(uvw);

            float E = dot(dx, dx);
            float F = dot(dx, dy);
            float G = dot(dy, dy);

            float Q = E + G;
            float R_sqr = (E * G) - (F * F); 

            float discriminantSqr = max(0.0f, Q * Q - 4.0f * R_sqr);
            float discriminant = sqrt(discriminantSqr);

            frequency.Max = sqrt((Q + discriminant) * 0.5f);
            frequency.Min = sqrt((Q - discriminant) * 0.5f);
            
            return frequency;
        }
    }
}