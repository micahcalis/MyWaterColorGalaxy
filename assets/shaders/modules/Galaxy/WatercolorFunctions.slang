#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Math.PartialDerivative;
import ColorSpace;

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
        public Math.PartialDerivative Derivative;
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

            // if (NdotL < FRESNEL_EPS)
            // {
            //     sampledOffset = BlurOffset(sampledOffset);
            // }

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

    public struct PaperGranulator
    {
        private float paperInverse;
        private float3 baseColor;
        private float granulationIntensity;
        private float pixelGranulation;

        public __init(float _paperHeight,
            float3 _baseColor,
            float _granulationIntensity,
            float _pixelGranulation)
        {
            paperInverse = 1.0f - _paperHeight;
            baseColor = _baseColor;
            granulationIntensity = _granulationIntensity;
            pixelGranulation = _pixelGranulation;
        }

        public float3 Granulate()
        {
            float3 I = baseColor; 
            float p = 1.0 - paperInverse; 
            
            float g = pixelGranulation * granulationIntensity; 
            float modulatedPiv = p * g; 
            float3 subtractionMode = I * saturate(I - modulatedPiv);
            
            float3 safeI = max(I, 0.0001);
            float3 transmittanceMode = (1.0 - I) * pow(safeI, 1.0 + modulatedPiv);

            return saturate(subtractionMode + transmittanceMode);
        }
    }

    public struct ColorBleeder<TWaterColorProcessingSampler : IWatercolorProcessingSampler>
    {
        private TWaterColorProcessingSampler wpSampler;
        private float colorBleed;

        public __init(TWaterColorProcessingSampler _wpSampler,
            float _colorBleed)
        {
            wpSampler = _wpSampler;
            colorBleed = _colorBleed;
        }

        public float3 GetColor(float2 uvOffset)
        {
            float3 base = wpSampler.SampleMain(uvOffset).rgb;
            float3 blur = wpSampler.SampleBlurred(uvOffset).rgb;

            return lerp(base, blur, saturate(colorBleed));
        }
    }

    public float2 GetPaperDistortionOffset(Math.PartialDerivative paperDerivative,
        float paperDistortionIntensity,
        float pixelDistortion)
    {
        return float2(paperDerivative.X, paperDerivative.Y) * paperDistortionIntensity * pixelDistortion;
    }

    public struct BleedComputer
    {
        private float depth;
        private float3 normalWS;
        private float3 viewDirWS;
        private float bleedMin;
        private float bleedMax;

        public __init(float3 _positionWS,
            float3 _normalWS,
            float _bleedMin,
            float _bleedMax)
        {
            float3 viewVector = _positionWS - _CameraPos;
            depth = length(viewVector);
            normalWS = _normalWS;
            viewDirWS = viewVector / max(depth, 0.0001f);
            bleedMin = _bleedMin;
            bleedMax = _bleedMax;
        }

        public float GetBleedValue()
        {
            float NdotV = saturate(dot(normalWS, -viewDirWS));
            float fresnel = 1.0f - smoothstep(0.0f, 1.5f, NdotV * NdotV *  NdotV);
            float depthGrad = smoothstep(bleedMin, bleedMax, depth);

            return depthGrad * fresnel;
        }
    }

    public struct EdgeDarkener<TWaterColorProcessingSampler : IWatercolorProcessingSampler>
    {
        private TWaterColorProcessingSampler wpSampler;
        private float3 bleededColor;
        private float edgeDarkening;

        public __init(TWaterColorProcessingSampler _wpSampler,
            float3 _bleededColor,
            float _edgeDarkening)
        {
            wpSampler = _wpSampler;
            bleededColor = _bleededColor;
            edgeDarkening = _edgeDarkening;
        }

        public float3 ComputeEdgeDarkening(float2 offset)
        {
            float3 base = wpSampler.SampleMain(offset).rgb;
            float3 blur = wpSampler.SampleBlurred(offset).rgb;
            float3 DoG = blur - base;
            float exponent = 1.0f + edgeDarkening * max(DoG.r, max(DoG.g, DoG.b));
            return pow(bleededColor, exponent);
        }
    }
}