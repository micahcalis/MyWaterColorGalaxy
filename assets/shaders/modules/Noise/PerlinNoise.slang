module PerlinNoise;

#define NOISE_SIMPLEX_1_DIV_289 0.00346020761245674740484429065744f

namespace Noise
{
    public struct Perlin
    {
        private int seed;
        private float2 uv;

        public __init(int _seed, float2 _uv)
        {
            seed = _seed;
            uv = _uv;
        }

        public float Compute(float2 frequency = 1)
        {
            float2 coord = uv * frequency;
            float2 floorCoord = floor(coord);
            float2 fracCoord = frac(coord);
            float d00 = dot(Functions.RandomVec2D(floorCoord, seed), fracCoord);
            float d01 = dot(Functions.RandomVec2D(floorCoord + float2(0, 1), seed), fracCoord - float2(0, 1));
            float d10 = dot(Functions.RandomVec2D(floorCoord + float2(1, 0), seed), fracCoord - float2(1, 0));
            float d11 = dot(Functions.RandomVec2D(floorCoord + float2(1, 1), seed), fracCoord - float2(1, 1));
            fracCoord = fracCoord * fracCoord * fracCoord * (fracCoord * (fracCoord * 6 - 15) + 10);
            float perlin = lerp(lerp(d00, d01, fracCoord.y), lerp(d10, d11, fracCoord.y), fracCoord.x);
            return perlin;
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

    public struct Perlin1D
    {
        private int seed;
        private float gradient;

        public __init(int _seed,
            float _gradient)
        {
            seed = _seed;
            gradient = _gradient;
        }

        public float Compute(float frequency = 1)
        {
            float coord = gradient * frequency;
            int floorCoord = (int)floor(coord);
            float fracCoord = frac(coord);

            float d0 = dot(Functions.RandomVec1D(floorCoord, seed), fracCoord);
            float d1 = dot(Functions.RandomVec1D(floorCoord + 1, seed), fracCoord);

            fracCoord = fracCoord * fracCoord * fracCoord * (fracCoord * (fracCoord * 6 - 15) + 10);
            float perlin = lerp(d0, d1, fracCoord);
            return perlin;
        }
    }

    public struct TileablePerlin3D
    {
        private int seed;
        private float3 uvw;

        public __init(int _seed,
            float3 _uvw)
        {
            seed = _seed;
            uvw = _uvw;
        }

        public float Compute(int3 frequency)
        {
            float3 coord = uvw * frequency;
            float3 Pi0 = floor(coord);
            float3 Pi1 = Pi0 + float3(1.0, 1.0, 1.0);
            float3 Pf0 = frac(coord);
            float3 Pf1 = Pf0 - float3(1.0, 1.0, 1.0);

            float3 fRep = (float3)frequency;
            if(fRep.x > 0.0) 
            {
                Pi0 = Functions.Modf3(Pi0, fRep);
                Pi1 = Functions.Modf3(Pi1, fRep);
            }
            
            Pi0 = Functions.Mod289f3(Pi0);
            Pi1 = Functions.Mod289f3(Pi1);
            
            float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
            float4 iy = float4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);
            float4 iz0 = Pi0.zzzz;
            float4 iz1 = Pi1.zzzz;

            float fSeed = (float)seed; 
            float4 ixy = Functions.Permute(Functions.Permute(ix) + iy + fSeed);
            
            float4 ixy0 = Functions.Permute(ixy + iz0);
            float4 ixy1 = Functions.Permute(ixy + iz1);

            float4 gx0 = ixy0 / 7.0;
            float4 gy0 = frac(floor(gx0) / 7.0) - 0.5;
            gx0 = frac(gx0);
            float4 gz0 = float4(0.5, 0.5, 0.5, 0.5) - abs(gx0) - abs(gy0);
            float4 sz0 = step(gz0, float4(0.0, 0.0, 0.0, 0.0));
            gx0 -= sz0 * (step(0.0, gx0) - 0.5);
            gy0 -= sz0 * (step(0.0, gy0) - 0.5);

            float4 gx1 = ixy1 / 7.0;
            float4 gy1 = frac(floor(gx1) / 7.0) - 0.5;
            gx1 = frac(gx1);
            float4 gz1 = float4(0.5, 0.5, 0.5, 0.5) - abs(gx1) - abs(gy1);
            float4 sz1 = step(gz1, float4(0.0, 0.0, 0.0, 0.0));
            gx1 -= sz1 * (step(0.0, gx1) - 0.5);
            gy1 -= sz1 * (step(0.0, gy1) - 0.5);

            float3 g000 = float3(gx0.x, gy0.x, gz0.x);
            float3 g100 = float3(gx0.y, gy0.y, gz0.y);
            float3 g010 = float3(gx0.z, gy0.z, gz0.z);
            float3 g110 = float3(gx0.w, gy0.w, gz0.w);
            float3 g001 = float3(gx1.x, gy1.x, gz1.x);
            float3 g101 = float3(gx1.y, gy1.y, gz1.y);
            float3 g011 = float3(gx1.z, gy1.z, gz1.z);
            float3 g111 = float3(gx1.w, gy1.w, gz1.w);

            float4 norm0 = Functions.TaylorInvSqrt(float4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
            g000 *= norm0.x; g010 *= norm0.y; g100 *= norm0.z; g110 *= norm0.w;
            float4 norm1 = Functions.TaylorInvSqrt(float4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
            g001 *= norm1.x; g011 *= norm1.y; g101 *= norm1.z; g111 *= norm1.w;

            float n000 = dot(g000, Pf0);
            float n100 = dot(g100, float3(Pf1.x, Pf0.y, Pf0.z));
            float n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
            float n110 = dot(g110, float3(Pf1.x, Pf1.y, Pf0.z));
            float n001 = dot(g001, float3(Pf0.x, Pf0.y, Pf1.z));
            float n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
            float n011 = dot(g011, float3(Pf0.x, Pf1.y, Pf1.z));
            float n111 = dot(g111, Pf1);

            float3 fade_xyz = Pf0 * Pf0 * Pf0 * (Pf0 * (Pf0 * 6.0 - 15.0) + 10.0);
            float4 n_z = lerp(float4(n000, n100, n010, n110), float4(n001, n101, n011, n111), fade_xyz.z);
            float2 n_yz = lerp(n_z.xy, n_z.zw, fade_xyz.y);
            float n_xyz = lerp(n_yz.x, n_yz.y, fade_xyz.x); 
            
            return 2.2 * n_xyz;
        }
    }

    namespace Functions
    {
        float2 Mod(float2 x, float2 y) {
            return x - y * floor(x / y);
        }

        float3 Modf3(float3 x, float3 y) 
        {
            return x - y * floor(x / y);
        }

        float Mod289(float x)
        {
            return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
        }

        float2 Mod289f2(float2 x)
        {
            return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
        }

        float3 Mod289f3(float3 x) 
        {
            return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
        }

        float4 Mod289f4(float4 x) 
        {
            return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
        }

        float3 Permutef3(float3 x) 
        {
            return Mod289f3(x * x * 34.0 + x);
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

        float RandomVec1D(float p, int seed)
        {
            p = Mod289(p + (float)seed);
            p = Mod289((34.0f * p + 1.0f) * p);

            float fraction = p / 289.0f;
            return (fraction * 2.0f) - 1.0f;
        }
    }
}