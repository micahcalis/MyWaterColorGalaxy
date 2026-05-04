#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Noise.Hash;

module WorleyNoise;

namespace Noise
{
    public struct Worley
    {
        private float2 uv;
        private int seed;

        public __init(float2 _uv, int _seed)
        {
            uv = _uv;
            seed = _seed;
        }

        public float Compute(float frequency = 1)
        {
            int scale = (int)frequency;
            int octaveSeed = seed + scale * 12345; 
            float2 gridOffset = Functions.Hash22(int2(octaveSeed, octaveSeed * 2)) * 100.0f;

            float2 scaledUv = (uv * scale) + gridOffset;
            
            int2 floorUv = (int2)floor(scaledUv);
            float2 fracUv = frac(scaledUv);
            float minDist = 1e+6f;

            [unroll(3)]
            for (int y = -1; y <= 1; y++)
            {
                [unroll(3)]
                for (int x = -1; x <= 1; x++)
                {
                    float2 lattice = float2(x, y);
                    int2 cellId = floorUv + int2(x, y);
                    
                    int2 wrappedCellId = (cellId % scale + scale) % scale; 
                    float2 point = Functions.Hash22(wrappedCellId + octaveSeed); 
                    
                    float2 difference = lattice + point - fracUv;
                    float dist = length(difference);
                    minDist = min(minDist, dist);
                }
            }

            return 1.0f - minDist;
        }
    }
}