module PerlinNoise;

namespace Noise
{
    public struct Perlin
    {
        private int seed;
        private float scale;
        private float exponent;
        private float2 uv;

        public __init(int _seed, float _scale, float _exponent, float2 _uv)
        {
            seed = _seed;
            scale = _scale;
            exponent = _exponent;
            uv = _uv;
        }

        public float Compute(float intensity)
        {
            float2 coord = uv * scale;
            float2 floorCoord = floor(coord);
            float2 fracCoord = frac(coord);
            float d00 = dot(Functions.RandomVec2D(floorCoord, seed), fracCoord);
            float d01 = dot(Functions.RandomVec2D(floorCoord + float2(0, 1), seed), fracCoord - float2(0, 1));
            float d10 = dot(Functions.RandomVec2D(floorCoord + float2(1, 0), seed), fracCoord - float2(1, 0));
            float d11 = dot(Functions.RandomVec2D(floorCoord + float2(1, 1), seed), fracCoord - float2(1, 1));
            fracCoord = fracCoord * fracCoord * fracCoord * (fracCoord * (fracCoord * 6 - 15) + 10);
            float perlin = lerp(lerp(d00, d01, fracCoord.y), lerp(d10, d11, fracCoord.y), fracCoord.x);
            return perlin * intensity;
        }
    };

    namespace Functions
    {
        float2 Mod289(float2 x)
        {
            return x - floor(x * (1.0 / 289.0)) * 289.0;
        }

        float Mod289(float x)
        {
            return x - floor(x * (1.0 / 289.0)) * 289.0;
        }

        float2 RandomVec2D(float2 p, int seed)
        {
            p = Mod289(p + float(seed));
            
            float x = Mod289((34.0 * p.x + 1.0) * p.x) + p.y;
            x = Mod289((34.0 * x + 1.0) * x);
            x = frac(x / 41.0) * 2.0 - 1.0;
            
            return normalize(float2(x - floor(x + 0.5), abs(x) - 0.5));
        }
    }
}