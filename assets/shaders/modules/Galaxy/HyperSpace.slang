#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Math.BeerMath;
import Noise.SimplexNoise;

module HyperSpace;

// shader based on: https://www.shadertoy.com/view/WdyXDm, by Louis Sugy

namespace Galaxy
{
    static const float DISK_SIZE = 0.02f;

    public interface IColorSampler
    {
        public float3 GetColor(float hash);
    }

    public struct HyperSpace<TColorSampler : IColorSampler>
    {
        private float2 uv;
        private float scale;
        private float aspectRatio;
        private float3 lightColor;
        private float3 darkColor;
        private  TColorSampler colorSampler;

        public __init(float2 _uv, 
            float _aspectRatio, 
            float _scale,
            float3 _lightColor,
            float3 _darkColor,
            TColorSampler _colorSampler)
        {
            uv = _uv;
            aspectRatio = _aspectRatio;
            scale = _scale;
            lightColor = _lightColor;
            darkColor = _darkColor;
            colorSampler = _colorSampler;
        }

        public float3 Resolve(float time)
        {
            float2 centerUv = (uv - 0.5f) * 2.0f;
            centerUv.x *= aspectRatio; 
            
            float2 scaledUv = centerUv * scale;
            float2 polarUv = float2(atan2(scaledUv.y, scaledUv.x), length(scaledUv));

            float t = saturate(time);

            float hash;
            float streak = GetStreak(polarUv, t, hash);
            float disk = GetDisk(polarUv, t);
            float fadeInAlpha = smoothstep(0.5f, 0.7f, t);

            float light = saturate(streak + disk);
            float3 hashColor = colorSampler.GetColor(hash);
            float centerMask = smoothstep(0.05f, 0.5f, polarUv.y);

            float3 patternCol = lerp(darkColor, hashColor, light * centerMask);
            float3 fadeCol = lerp(patternCol, lightColor, fadeInAlpha);

            return fadeCol;
        }

        private float GetStreak(float2 polarUv, float t, out float hash)
        {
            float stretchedRadius = polarUv.y * 40.0f / (5.0f + 60.0f * Math.JumpStep(0.0f, 2.0f, 0.5f * pow(t * 2.0f, 3.5f)));

            float noiseVal = Noise.SimplexNoise(float3(60.0f * polarUv.x, stretchedRadius, 0.0f), hash);

            float noiseGain = 1.0f + 2.0f * smoothstep(0.5f, 0.9f, t);
            noiseVal *= noiseGain;

            float noiseTime = saturate(lerp(0.25f, 0.0f, t));
            noiseVal = smoothstep(noiseTime, 1.0f, noiseVal);

            float streakIntensity = lerp(0.0f, 10.0f, t * 1.5f);
            noiseVal *= polarUv.y * streakIntensity * noiseVal;

            float whiteLevel = smoothstep(0.0f, 1.0f, noiseVal);
            whiteLevel *= whiteLevel;

            return saturate((streakIntensity * noiseVal) + whiteLevel);
        }

        private float GetDisk(float2 polarUv, float t)
        {
            float diskIntensity = smoothstep(0.25f, 0.65f, t);
            float diskMask = exp(-(polarUv.y - DISK_SIZE) * 3.0f);
            return saturate(diskIntensity * diskMask);
        }
    }
}