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

    public struct TileablePerlin
    {
        private int seed;
        private float2 uv;

        public __init(int _seed, float2 _uv)
        {
            seed = _seed;
            uv = _uv;
        }

        public float Compute(int2 frequency)
        {
            float2 Pi0 = floor(uv * frequency);
            float2 Pi1 = Pi0 + float2(1.0, 1.0);
            float2 Pf0 = frac(uv * frequency);
            float2 Pf1 = Pf0 - float2(1.0, 1.0);
            
            float2 fRep = (float2)frequency;
            if(fRep.x > 0.0) 
            {
                Pi0 = Functions.Mod(Pi0, fRep);
                Pi1 = Functions.Mod(Pi1, fRep);
            }
            
            Pi0 = Functions.Mod(Pi0, float2(289.0, 289.0));
            Pi1 = Functions.Mod(Pi1, float2(289.0, 289.0));
            
            float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
            float4 iy = float4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);

            float fSeed = (float)seed; 
            
            float4 ixy = Functions.Permute(Functions.Permute(ix) + iy + fSeed);

            float4 gx = frac(ixy * (1.0 / 41.0)) * 2.0 - 1.0;
            float4 gy = abs(gx) - 0.5;
            float4 tx = floor(gx + 0.5);
            gx = gx - tx;

            float2 g00 = float2(gx.x, gy.x);
            float2 g10 = float2(gx.y, gy.y);
            float2 g01 = float2(gx.z, gy.z);
            float2 g11 = float2(gx.w, gy.w);

            float4 norm = Functions.TaylorInvSqrt(float4(dot(g00, g00), dot(g10, g10), dot(g01, g01), dot(g11, g11)));
            g00 *= norm.x; 
            g10 *= norm.y; 
            g01 *= norm.z; 
            g11 *= norm.w;

            float n00 = dot(g00, float2(Pf0.x, Pf0.y));
            float n10 = dot(g10, float2(Pf1.x, Pf0.y));
            float n01 = dot(g01, float2(Pf0.x, Pf1.y));
            float n11 = dot(g11, float2(Pf1.x, Pf1.y));

            float2 fade_xy = Pf0 * Pf0 * Pf0 * (Pf0 * (Pf0 * 6.0 - 15.0) + 10.0);
            
            float2 n_x = lerp(float2(n00, n01), float2(n10, n11), fade_xy.x);
            float n_xy = lerp(n_x.x, n_x.y, fade_xy.y); 
            
            return 2.3 * n_xy;
        }
    }

    namespace Functions
    {
        float2 Mod(float2 x, float2 y) {
            return x - y * floor(x / y);
        }

        float Mod289(float x)
        {
            return x - floor(x * (1.0 / 289.0)) * 289.0;
        }

        float2 Mod289f2(float2 x)
        {
            return x - floor(x * (1.0 / 289.0)) * 289.0;
        }

        float4 Mod289f4(float4 x) 
        {
            return x - floor(x * (1.0 / 289.0)) * 289.0;
        }

        float4 Permute(float4 x) 
        {
            return Mod289f4(((x * 34.0) + 1.0) * x);
        }

        float4 TaylorInvSqrt(float4 r) 
        {
            return 1.79284291400159 - 0.85373472095314 * r;
        }

        float2 RandomVec2D(float2 p, int seed)
        {
            p = Mod289f2(p + float(seed));
            
            float x = Mod289((34.0 * p.x + 1.0) * p.x) + p.y;
            x = Mod289((34.0 * x + 1.0) * x);
            x = frac(x / 41.0) * 2.0 - 1.0;
            
            return normalize(float2(x - floor(x + 0.5), abs(x) - 0.5));
        }
    }
}