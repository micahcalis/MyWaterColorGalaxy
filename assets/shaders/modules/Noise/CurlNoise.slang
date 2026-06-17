#include "Core/BeerMacros.slang"

import Core.BeerCore;

module CurlNoise;

namespace Noise
{
    enum CurlDirection : uint
    {
        Left = 0,
        Right = 1,
        Down = 2,
        Up = 3,
        Back = 4,
        Forward = 5
    }

    static const int3[6] CURL_OFFSETS = {
        int3(-1, 0, 0),
        int3(1, 0, 0),
        int3(0, -1, 0),
        int3(0, 1, 0),
        int3(0, 0, -1),
        int3(0, 0, 1)
    };

    static const int3 GetCurlOffset(CurlDirection dir)
    {
        return CURL_OFFSETS[dir];
    }

    public interface ICurlSampler
    {
        public float3 Sample(int3 offset);
    }

    public struct Curl<TCurlSampler : ICurlSampler>
    {
        private TCurlSampler curlSampler;

        public __init(TCurlSampler _curlSampler)
        {
            curlSampler = _curlSampler;
        }

        public float3 Compute(float stepSize = 1.0f)
        {
            float3 left = curlSampler.Sample(GetCurlOffset(CurlDirection::Left));
            float3 right = curlSampler.Sample(GetCurlOffset(CurlDirection::Right));
            float3 down = curlSampler.Sample(GetCurlOffset(CurlDirection::Down));
            float3 up = curlSampler.Sample(GetCurlOffset(CurlDirection::Up));
            float3 back = curlSampler.Sample(GetCurlOffset(CurlDirection::Back));
            float3 forward = curlSampler.Sample(GetCurlOffset(CurlDirection::Forward));

            float3 curl;
            curl.x = up.z - down.z - forward.y + back.y;
            curl.y = forward.x - back.x - right.z + left.z;
            curl.z = right.y - left.y - up.x + down.x;

            float denominator = 2.0f * stepSize;

            return curl * rcp(denominator);
        }
    }
}