#include "Core/BeerMacros.slang"

import Noise.Hash;

module VoronoiNoise;

namespace Noise
{
    public struct VoronoiResult
    {
        public float Distance;
        public int2 Id;
        public float2 Center;
    }

    public struct Voronoi
    {
        private float2 uv;
        private int seed;

        public __init(float2 _uv, int _seed)
        {
            uv = _uv;
            seed = _seed;
        }

        public VoronoiResult Compute(float2 frequency = 1)
        {
            float2 scaledUv = uv * frequency;
            int2 floorUv = (int2)scaledUv;
            float2 fracUv = frac(scaledUv);
            VoronoiResult result = (VoronoiResult)0;
            result.Distance = 1e+6f;

            [unroll(3)]
            for (int y = -1; y <= 1; y++)
            {
                [unroll(3)]
                for (int x = -1; x <= 1; x++)
                {
                    float2 lattice = float2(x, y);
                    int2 cellId = floorUv + int2(x, y);

                    float2 point = Functions.Hash22(cellId + seed);
                    float2 difference = lattice + point - fracUv;
                    float dist = length(difference);

                    if (dist < result.Distance)
                    {
                        result.Distance = dist;
                        result.Id = cellId;
                        result.Center = cellId + point;
                    }
                }
            }

            return result;
        }
    }

    public struct Voronoi1D
    {
        private float gradient;
        private int seed;

        public __init(float _gradient, int _seed)
        {
            gradient = _gradient;
            seed = _seed;
        }

        public float Compute(float frequency = 1, float angle = 1)
        {
            float scaledGrad = gradient * frequency;
            int floorGrad = (int)scaledGrad;
            float fracGrad = frac(scaledGrad);
            float minDist = 1e+6f;

            [unroll(3)]
            for (int i = -1; i <= 1; i++)
            {
                int cellId = floorGrad + i;
                float point = Functions.Hash11(cellId, seed) * angle;
                float difference = abs(i + point - fracGrad);
                minDist = min(minDist, difference);
            }

            return minDist;
        } 
    }

    public struct Voronoi1DClamped
    {
        private float gradient;
        private int seed;
        private int2 range;

        public __init(float _gradient, 
            int _seed,
            int2 _range)
        {
            gradient = _gradient;
            seed = _seed;
            range = _range;
        }

        public float Compute(float frequency = 1, float angle = 1)
        {
            float scaledGrad = gradient * frequency;
            int floorGrad = (int)scaledGrad;
            float fracGrad = frac(scaledGrad);
            float minDist = 1e+6f;

            [unroll(3)]
            for (int i = -1; i <= 1; i++)
            {
                int cellId = floorGrad + i;
                float point = Functions.Hash11(cellId, seed) * angle;
                bool inRange = cellId >= range.x && cellId <= range.y;
                float difference = inRange ? abs(i + point - fracGrad) : 1e+6f;
                minDist = min(minDist, difference);
            }

            return minDist;
        } 
    }
}