#include "Core/BeerMacros.slang"

import Core.BeerCore;

module WatercolorFunctions;

namespace WC
{
    public float ApplyBloat(float scale, float3 position, float bloatModifier)
    {
        float scaledDist = distance(position, _CameraPos);
        return scale + sqrt(scaledDist) * bloatModifier;
    }

    public struct NormalsOffset
    {
        public float3 Normal;
        public float2 Offset;
    }

    public struct WatercolorParams
    {
        public float PaperDistortion;
        public float PaperGranulation;
        public float ColorBleed;
        public float PigmentTurbulence;
    }

    public struct PaperData
    {
        public float2 Slope;
        public float Height;
    }

    public interface IWatercolorProcessingSampler
    {
        public NormalsOffset SampleNormalsOffset(float2 offset);
        public WatercolorParams SampleParams(float2 offset);
        public float4 SampleMain(float2 offset);
        public float4 SampleBlurred(float2 offset);
        public PaperData SamplePaper(float2 offset);
        public float SampleDepth(float2 offset);
    }

    public struct TremorOffsetter<TWatercolorProcessingSampler : IWatercolorProcessingSampler>
    {
        private static const float FRESNEL_EPS = 0.25f;
        private static const float2 LEFT = int2(-1, 0);
        private static const float2 RIGHT = int2(1, 0);
        private static const float2 UP = int2(0, 1);
        private static const float2 DOWN = int2(0, -1);
        private static const float2[4] BLUR_DIR = {
            LEFT,
            RIGHT,
            UP,
            DOWN };
        
        private TWatercolorProcessingSampler wpSampler;
        private float offsetIntensity;
        private float depthThreshold;
        private float3 viewDirWS;
        
        public __init(TWatercolorProcessingSampler _wpSampler,
            float _offsetIntensity,
            float _depthThreshold,
            float3 _viewDirWS)
        {
            wpSampler = _wpSampler;
            offsetIntensity = _offsetIntensity;
            depthThreshold = _depthThreshold;
            viewDirWS = _viewDirWS;
        }

        public float2 GetOffset()
        {
            NormalsOffset normalsOffset = wpSampler.SampleNormalsOffset(0);

            float2 sampledOffset = normalsOffset.Offset * offsetIntensity * float2(GetAspect(), 1);
            float NdotL = dot(normalsOffset.Normal, -viewDirWS);

            if (NdotL < FRESNEL_EPS)
            {
                sampledOffset = BlurOffset(sampledOffset);
            }

            // float currentDepth = GetLinearDepth(wpSampler.SampleDepth(0));
            // float offsetDepth = GetLinearDepth(wpSampler.SampleDepth(sampledOffset));
            // float depthDifference = abs(currentDepth - offsetDepth);
            // float depthFade = 1.0 - smoothstep(0.0, depthThreshold, depthDifference);

            return sampledOffset;
        }

        private float2 BlurOffset(float2 baseOffset)
        {
            float2 texelSize = GetScreenTexelSize();

            [unroll(4)]
            for (int i = 0; i < 4; i++)
            {
                baseOffset += wpSampler.SampleNormalsOffset(BLUR_DIR[i]).Offset;
            }

            return baseOffset / 5.0f;
        }
    }
}