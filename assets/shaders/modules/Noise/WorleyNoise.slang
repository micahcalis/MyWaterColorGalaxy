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

    public struct Worley3D
    {
        private float3 uvw; 
        private int seed;

        public __init(float3 _uvw,
            int _seed)
        {
            uvw = _uvw;
            seed = _seed;
        }

        public float Compute(float3 frequency = 1)
        {
            int3 scale = (int3)frequency;
            float3 scaledCoord = uvw * scale;
            int3 floorCoord = (int3)floor(scaledCoord);
            float3 fracCoord = frac(scaledCoord);

            float minDist = 1e+6f;

            [unroll]
            for (int z = -1; z <= 1; z++)
            {
                [unroll]
                for (int y = -1; y <= 1; y++)
                {
                    [unroll]
                    for (int x = -1; x <= 1; x++)
                    {
                        float3 neighbour = float3(x, y, z);
                        int3 cellId = floorCoord + int3(x, y, z);
                        int3 wrappedCellId = (cellId % scale + scale) % scale;
                        float3 randomPoint = Functions.Hash33(wrappedCellId + seed);
                        float3 diff = neighbour + randomPoint - fracCoord;
                        float dist = length(diff);
                        minDist = min(minDist, dist);
                    }
                }
            }

            return 1.0f - minDist;
        }
    }
}