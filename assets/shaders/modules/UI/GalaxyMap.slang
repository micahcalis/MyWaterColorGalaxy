#include "Core/BeerMacros.slang"

import Noise.VoronoiNoise;
import Noise.Hash;
import Noise.PerlinNoise;
import SDF;
import ColorSpace;
import Math.BeerMath;
import Math.Effects;
import Math.Easings;

module GalaxyMap;

namespace Map
{
    static const int MIN_STAR_NUMBER = 4;
    static const int STAR_VAR_AMOUNT = 4;
    static const float[STAR_VAR_AMOUNT] SHARPNESS_MAP =
    {
        3.2f,
        3.7f,
        4.2f,
        5.4f
    };

    static const float PULL_TO_CENTER_AMOUNT = 0.15f;

    public struct Stars
    {
        private Noise.Voronoi voronoi;
        private float2 uv;
        private int seed;
        private float2 frequency;
        private float starScale;
        private float4 starColor;

        public __init(int _seed,
            float2 _uv,
            float2 _frequency,
            float _starScale,
            float4 _starColor)
        {
            voronoi = Noise.Voronoi(_uv, _seed);
            uv = _uv;
            seed = _seed;
            frequency = _frequency;
            starScale = _starScale;
            starColor = _starColor;
        }

        public float4 Get()
        {
            Noise.VoronoiResult noise = voronoi.Compute(frequency);
            float starHash = Noise.Functions.Hash21(noise.Id + seed);
            float starRadius = starScale * starHash;

            int number; float sharpness;
            GetStarProperties(starHash, number, sharpness);

            float2 localPoint = noise.Center - noise.Id;
            float2 safeLocalPoint = lerp(localPoint, float2(0.5f, 0.5f), PULL_TO_CENTER_AMOUNT);
            float2 safeCenter = noise.Id + safeLocalPoint;
            SDF.Star star = SDF.Star(safeCenter, starRadius, number, sharpness);

            float starMask = smoothstep(0.01f, -starScale, star.Evaluate(uv * frequency));
            return float4(starMask * starColor.rgb, starMask);
        }

        private void GetStarProperties(float hash, out int number, out float sharpness)
        {
            int rawNumber = (int)(hash * 12347.45f) % STAR_VAR_AMOUNT;
            number = rawNumber + MIN_STAR_NUMBER;
            sharpness = SHARPNESS_MAP[rawNumber];
        }
    }   

    static const int DUST_CLOUD_NUMBER = 3;

    public interface IStarDustNoiseSampler
    {
        public float GetNoise(float2 uv);
    }

    public struct StarDust<TNoiseSampler : IStarDustNoiseSampler>
    {
        private TNoiseSampler noiseSampler; 
        private float2 uv;
        private int seed;
        private float2 frequency;
        private float3[DUST_CLOUD_NUMBER] colors;
        private float threshold;
        private float edgeThickness;
        private int posterizeSteps;
        private float opacity;

        public __init(TNoiseSampler _noiseSampler,
            float2 _uv,
            int _seed,
            float2 _frequency,
            float3[DUST_CLOUD_NUMBER] _colors,
            float _threshold,
            float _edgeThickness,
            int _posterizeSteps,
            float _opacity)
        {
            noiseSampler = _noiseSampler;
            uv = _uv;
            seed = _seed;
            frequency = _frequency;
            colors = _colors;
            threshold = _threshold;
            edgeThickness = _edgeThickness;
            posterizeSteps = _posterizeSteps;
            opacity = _opacity;
        }

        public float4 Get()
        {
            float3 subtractiveHue = float3(1.0, 1.0, 1.0);
            float totalDensity = 0.0;

            [unroll(DUST_CLOUD_NUMBER)]
            for (int i = 0; i < DUST_CLOUD_NUMBER; i++)
            {
                float mask = CalculateDustMask(seed + i);
                totalDensity += mask;
                subtractiveHue *= lerp(float3(1.0, 1.0, 1.0), colors[i], mask);
            }
            
            float opacity = saturate(totalDensity);

            return float4(subtractiveHue * opacity, opacity);
        }

        private float CalculateDustMask(int index)
        {
            float2 seedOffset = float2(Noise.Functions.IntegerHash(index),
                Noise.Functions.IntegerHash(index + 1));

            float noiseMask = noiseSampler.GetNoise(uv + seedOffset); 
            float outerShape = smoothstep(threshold - edgeThickness, threshold, noiseMask); 
            float innerShape = smoothstep(threshold, threshold + edgeThickness, noiseMask);
            float edge = outerShape - innerShape;

            float posterizeNoise = Math.Posterize(noiseMask, 9);

            return saturate(posterizeNoise * opacity * innerShape + edge * opacity);
        }
    }

    static const float STRETCH_MIN = 0.8f;
    static const float STRETCH_MAX = 1.2f;
    static const float AA_RING_EPS = 0.1f;

    public struct Rings
    {
        private float2 center;
        private float2 uv;
        private int seed;
        private int numRings;
        private float thickness;
        private float3 color;

        public __init(float2 _center,
            float2 _uv,
            int _seed,
            int _numRings,
            float _thickness,
            float3 _color)
        {
            center = _center;
            uv = _uv;
            seed = _seed;
            numRings = _numRings;
            thickness = _thickness;
            color = _color;
        }

        public float4 Get()
        {
            float2 centerUv = uv - center;
            float2 stretch = float2(Noise.Functions.IntegerHash(seed + 9), 
                Noise.Functions.IntegerHash(seed - 5));

            stretch = float2(Math.Remap(stretch.x, float2(0.0f, 1.0f), float2(STRETCH_MIN, STRETCH_MAX)),
                Math.Remap(stretch.y, float2(0.0f, 1.0f), float2(STRETCH_MIN, STRETCH_MAX)));

            float2 scaledUv = centerUv * stretch * (float)numRings * 2.3f;

            Noise.Perlin perlin = Noise.Perlin(seed, uv * 80.0f);
            float2 uvOffset = perlin.Compute();
            scaledUv += uvOffset * 0.02f;

            float dist = length(scaledUv);

            Noise.Voronoi1DClamped voronoi1D = Noise.Voronoi1DClamped(dist, seed, int2(1, numRings));
            float noiseGrad = voronoi1D.Compute(1.0f, 0.5f);
            
            float ringMask = smoothstep(thickness, 0, noiseGrad);
            ringMask = smoothstep(1.0f - AA_RING_EPS, 1.0f, ringMask);
            ringMask *= saturate(pow(length(uvOffset), 1.5) * 4.0f + 0.65f);

            return float4(ringMask * color, ringMask);
        }
    }

    static const float AA_SUN_EPS = 0.02f;
    
    public struct Sun<TNoiseSampler : IStarDustNoiseSampler>
    {
        private TNoiseSampler noiseSampler;
        private float2 uv;
        private int seed;
        private float3 color;
        private float size;
        private float gradientIntensity;
        private float edgeIntensity;
        private int gradientSteps;

        public __init(TNoiseSampler _noiseSampler,
            float2 _uv,
            int _seed,
            float3 _color,
            float _size,
            float _gradientIntensity,
            float _edgeIntensity,
            int _gradientSteps)
        {
            noiseSampler = _noiseSampler;
            uv = _uv;
            seed = _seed;
            color = _color;
            size = _size;  
            gradientIntensity = _gradientIntensity;
            edgeIntensity = _edgeIntensity;
            gradientSteps = _gradientSteps;
        }

        public float4 Get()
        {
            float2 centerUv = uv - 0.5f;
            float sizeGrad = smoothstep(0, size, length(centerUv));

            float mask = GetMask(sizeGrad);
            float gradient = GetPosterizeGrad(sizeGrad);
            float3 mixColor = GetMixColor(gradient);

            return float4(mixColor, mask);
        }

        private float GetMask(float sizeGrad)
        {
            float mask = smoothstep(0.99f, 0.99f - AA_SUN_EPS, sizeGrad);
            return mask;
        }

        private float GetPosterizeGrad(float sizeGrad)
        {
            float2 centerUv = uv - 0.5f;
            float2 normalizedUv = centerUv * (0.5f / size);
            float2 sphereCoord = Math.SpherizeUV(normalizedUv, 0, 20, 0);

            float2 seedOffset = float2(Noise.Functions.IntegerHash(seed),
                Noise.Functions.IntegerHash(seed + 1));

            float noise = noiseSampler.GetNoise(sphereCoord + seedOffset);
            float posterizeNoise = Math.Posterize(noise, gradientSteps);

            float edgeRing = smoothstep(0.9f, 0.98f, sizeGrad);

            return posterizeNoise + edgeRing * edgeIntensity;
        }

        private float3 GetMixColor(float gradient)
        {
            float inkThickness = (gradient * gradientIntensity) + 1.0f; 
            return pow(color.rgb, inkThickness);
        }
    }

    static const float AA_FLARES_EPS = 0.08f;

    public struct Flares
    {
        private float2 uv;
        private int seed;
        private float3 color;
        private float size;
        private int flaresPerUnit;
        private float gradientIntensity;

        public __init(float2 _uv,
            int _seed,
            float3 _color,
            float _size,
            int _flaresPerUnit,
            float _gradientIntensity)
        {
            uv = _uv;
            seed = _seed;
            color = _color;
            size = _size;
            flaresPerUnit = _flaresPerUnit;
            gradientIntensity = _gradientIntensity;
        }

        public float4 Get()
        {
            int id;
            float circularGradient = GetCircularGradient(id);
            float mask;
            float distanceGradient = GetDistanceGradient(id, mask);

            float2 coord = float2(circularGradient, distanceGradient);
            float leafDist = length(coord);
            float leafMask = smoothstep(0.51f, 0.51f + AA_FLARES_EPS, leafDist);
            float leafGradient = pow(smoothstep(1.0f, 0.51, leafDist), 5.0f);

            return float4(GetMixColor(leafGradient), leafMask * mask);
        }

        private float GetCircularGradient(out int id)
        {
            float2 centerUv = uv - 0.5f;
            float angle = atan2(centerUv.x, centerUv.y);
            angle += PI;
            angle /= TAU;

            int flares = (int)((float)flaresPerUnit * size);
            float rawGrad = angle * (float)flares;
            float fracFlares = frac(rawGrad);
            id = (int)(rawGrad);

            Noise.Perlin perlin = Noise.Perlin(seed, float2(rawGrad, 0));
            float noise = perlin.Compute(10.0f) * 0.1f;

            return fracFlares - 0.5f + noise;
        }

        private float GetDistanceGradient(int id, out float mask)
        {
            float2 centerUv = uv - 0.5f;
            float rawDist = length(centerUv);
            rawDist += Noise.Functions.IntegerHash(seed + id) * 0.04f;
    
            float scaledDist = rawDist * 1.88f * rcp(size);

            mask = step(scaledDist, PI);

            float trigDist = cos(scaledDist) * 0.5f + 0.5f;
            return pow(trigDist, 0.42f);
        }

        private float3 GetMixColor(float gradient)
        {
            float inkThickness = (gradient * gradientIntensity) + 1.0f; 
            return pow(color.rgb, inkThickness);
        }
    }
}