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
    }
}