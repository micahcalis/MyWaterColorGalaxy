#include "Core/BeerMacros.slang"

import Core.BeerCore;

module FBM;

namespace Noise
{
    public interface INoiseComputer
    {
        public float GetNoise(float frequency);
    }

    public struct FBM<TNoiseComputer : INoiseComputer>
    {
        private int depth;
        private float frequency;
        private float frequencyMultiplier;
        private float amplitude;
        private float amplitudeMultiplier;

        private TNoiseComputer noiseComputer;

        public __init(int _depth,
            float _frequency,
            float _frequencyMultiplier,
            float _amplitude,
            float _amplitudeMultiplier)
        {
            depth = _depth;
            frequency = _frequency;
            frequencyMultiplier = _frequencyMultiplier;
            amplitude = _amplitude;
            amplitudeMultiplier = _amplitudeMultiplier;
        }

        [mutating]
        public void SetNoiseComputer(TNoiseComputer _noiseComputer)
        {
            noiseComputer = _noiseComputer;
        }

        public float Solve()
        {
            float total = 0;
            float f = frequency;
            float a = amplitude;
            float aSum = 1e-5f;

            for (int i = 0; i < depth; i++)
            {
                total += noiseComputer.GetNoise(f) * a;
                aSum += a;

                f *= frequencyMultiplier;
                a *= amplitudeMultiplier;
            }

            return total / aSum;
        }
    }
}