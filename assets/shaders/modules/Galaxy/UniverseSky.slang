#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Samplers;

module UniverseSky;

namespace Galaxy
{
    static const int NUM_DUST = 3;

    struct DustPair
    {
        float baseDust = 0;
        float blurDustA = 0;
        float blurDustB = 0;
        float3 color = 0;
        float opacity = 0;
    }

    public interface IDustSampler
    {
        public float3 SampleBase(float3 dir);
        public float3 SampleBlurA(float3 dir);
        public float3 SampleBlurB(float3 dir);
    }

    public struct DustResolver<TDustSampler : IDustSampler>
    {
        IDustSampler dustSampler;
        float3 dir;
        DustPair[NUM_DUST] dustPairs = (DustPair[NUM_DUST])0;
        float bleedThreshold;

        public __init(IDustSampler _dustSampler,
            float3 _dir,
            float4[NUM_DUST] _dustColors,
            float _bleedThreshold)
        {
            dustSampler = _dustSampler;
            dir = _dir;
            bleedThreshold = _bleedThreshold;

            float3 baseDust = dustSampler.SampleBase(dir);
            float3 blurDustA = dustSampler.SampleBlurA(dir);
            float3 blurDustB = dustSampler.SampleBlurB(dir);

            [unroll(NUM_DUST)]
            for (int i = 0; i < NUM_DUST; i++)
            {
                dustPairs[i].baseDust = baseDust[i];
                dustPairs[i].blurDustA = blurDustA[i];
                dustPairs[i].blurDustB = blurDustB[i];
                dustPairs[i].color = _dustColors[i].rgb;
                dustPairs[i].opacity = _dustColors[i].a;
            }  
        }

        public float4 ComputeDust()
        {
            float4 colorSum = 0;

            [unroll(NUM_DUST)]
            for (int i = 0; i < NUM_DUST; i++)
            {
                DustPair pair = dustPairs[i];
                float alpha = pair.opacity * ComputeBleedMask(dustPairs[i]) * pair.blurDustB;
                colorSum += float4(pair.color * alpha, alpha);
            }

            return colorSum;
        }

        private float ComputeBleedMask(DustPair pair)
        {
            float DoG = pair.blurDustA - pair.blurDustB;
            float exponent = 1.0f + DoG;
            float basePow = pow(pair.baseDust, exponent);

            return basePow > bleedThreshold ? 1.0f : 0.0f;
        }
    }
}