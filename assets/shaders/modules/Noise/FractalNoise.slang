#include "Core/BeerMacros.slang"

import Noise.FractalScale;

module FractalNoise;

namespace Noise
{
    public interface INoise3DSampler
    {
        public float Sample(float3 uvw);
    }

    public struct FractalNoise<TNoiseSampler : INoise3DSampler>
    {
        private static float BASE_SCALE = 9.0f;

        TNoiseSampler noiseSampler;
        float3 relativeWorldCoord;
        float fractalScale;
        
        public __init(TNoiseSampler _noiseSampler,
            float3 _positionWS,
            float3 _transformCenter,
            float frequency)
        {
            noiseSampler = _noiseSampler;
            relativeWorldCoord = _positionWS - _transformCenter;
            fractalScale = BASE_SCALE / frequency;
        }

        public float Solve(float3 offset = float3(0))
        {
            FractalData data = GetData();
            float3 coord = relativeWorldCoord + offset;

            float3 coordLow = coord / data.LowLevel;
            float3 coordHigh = coord / data.HighLevel;
            
            float3 patternLow = frac(coordLow);
            float turbulenceLow = noiseSampler.Sample(patternLow);
            float3 patternHigh = frac(coordHigh);
            float turbulenceHigh = noiseSampler.Sample(patternHigh);

            return lerp(turbulenceLow, turbulenceHigh, data.SubLayer);
        }

        private FractalData GetData()
        {
            Noise.FractalSolver fractalSolver = Noise.FractalSolver(relativeWorldCoord, fractalScale);
            Noise.FractalData fractalData = fractalSolver.Solve();
            fractalData.LowLevel = max(1, fractalData.LowLevel);
            fractalData.HighLevel = max(1, fractalData.HighLevel);

            return fractalData;
        }
    };
}