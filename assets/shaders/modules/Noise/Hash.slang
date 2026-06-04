#include "Core/BeerMacros.slang"

import Core.BeerCore;

module Hash;

namespace Noise
{
    namespace Functions
    {
        // integer hash copied from Hugo Elias
        public float IntegerHash(uint n) 
        {
            n = (n << 13U) ^ n;
            n = n * (n * n * 15731U + 0x789221U) + 0x1376312589U;
            return float(n & uint(0x7fffffffU)) / float(0x7fffffff);
        }

        public float Hash11(int p, int seed)
        {
            uint combined = uint(p) * 1619U ^ uint(seed) * 31337U;
            return IntegerHash(combined);
        }

        public float Hash21(int2 p)
        {
            uint combined = uint(p.x) * 1619U ^ uint(p.y) * 31337U;
            return IntegerHash(combined);
        }

        public float2 Hash22(int2 p)
        {
            float2 f = float2(IntegerHash(p.x), IntegerHash(p.y));
            
            f += dot(f, f.yx + 19.19);
            return frac((f.xx + f.yy) * f.yx);
        }

        public float3 Hash33(int3 p)
        {
            float3 f = frac(p * float3(IntegerHash(p.x), IntegerHash(p.y), IntegerHash(p.z)));
            f += dot(f, f.yzx + 19.19);
            return frac((f.xxy + f.yzz) * f.zyx);
        }

       public  float2 RandomVector2D(float2 id, float seed)
        {
            float2x2 m = float2x2(15.27, 47.63, 99.41, 89.98);
            float2 uv = frac(sin(mul(id, m)) * (46839.32 + seed));
            return normalize(uv * 2.0 - 1.0);
        }

        public int HashInt21(int2 seed)
        {
            uint n = uint(seed.x) * 1619U ^ uint(seed.y) * 31337U;

            n = (n << 13U) ^ n;
            n = n * (n * n * 15731U + 0x789221U) + 0x1376312589U;
            
            return int(n & uint(0x7fffffffU));
        }

        public float3 Hash13(int p, int seed)
        {
            uint p_u = uint(p);
            uint s_u = uint(seed);

            uint hx = p_u * 1619U ^ s_u * 31337U;
            uint hy = p_u * 31337U ^ s_u * 6971U;
            uint hz = p_u * 6971U ^ s_u * 2893U;
            
            return float3(
                IntegerHash(hx), 
                IntegerHash(hy), 
                IntegerHash(hz)
            );
        }
    }
}