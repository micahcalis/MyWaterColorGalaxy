#include "Core/BeerMacros.slang"

import Core.BeerCore;
import SDF;
import Noise.Hash;
import Math.BeerMath;
import Math.Gradient;

module GalaxySDFs;

namespace Galaxy
{
    const static int[5] PLANET_SEEDS = { 240, 30, 324, 23, 68 };
    const static float PLANET_RING_THICKNESS = 0.01f;
    const static float2 OUTER_RING_SIZE = float2(0.95f, 1.8f);
    const static float2 INNER_RING_SIZE = float2(1.05f, 1.3f);
    const static float MIN_RING_THICKNESS = 0.15f;
    public const static float PLANET_MAT = 0.0f;
    public const static float RING_MAT = 1.0f;
    public const static float3 PLANET_NOISE_SCALE = float3(0.1f, 1.2, 0.1f);
    public const static float RING_NOISE_SCALE = 3.0f;

    public interface IPlanetNoiseReader
    {
        public float Read(float3 uvw, int index);
    }

    public struct Planet : SDF.SDF
    {
        private float3 center;
        private float scale;
        private float invScale;
        private float radius;
        private float[5] hash;
        private float3 normal;
        private float3x3 basis;
        private float4[4] colors;

        public __init(float3 _center, 
            float _scale,
            int _seed,
            float4[4] _colors)
        {
            center = _center;
            scale = _scale;
            colors = _colors;
            invScale = rcp(scale);
            radius = 0.8f;
    
            [unroll(5)]
            for (int i = 0; i < 5; i++)
            {
                hash[i] = Noise.Functions.IntegerHash(_seed + PLANET_SEEDS[i]);
            }

            normal = normalize(float3(hash[2] - 0.5f, hash[3] - 0.5f, hash[4] - 0.5f));
            basis = Math.GetOrthogonalBasis(normal);
        }

        public SDF.FieldOut Evaluate(SDF.MarchData data)
        {
            float3 localPos = (data.Position - center) * invScale;
            SDF.FieldOut planetDist = Planet(localPos);
            planetDist.SignedDist *= scale;
            return planetDist;
        }

        private SDF.FieldOut Planet(float3 localPos)
        {
            SDF.FieldOut base = GetBase(localPos);
            SDF.FieldOut ring = GetRing(localPos);

            return SDF.Functions.Union(base, ring);
        }

        private SDF.FieldOut GetBase(float3 localPos)
        {
            float baseDist = SDF.Functions.Sphere(localPos, radius);
            return SDF.FieldOut(baseDist, PLANET_MAT);
        }

        private SDF.FieldOut GetRing(float3 localPos)
        {
            float plane = SDF.Functions.Box(mul(basis, localPos), float3(100, PLANET_RING_THICKNESS, 100));

            float innerRingSize = lerp(INNER_RING_SIZE.x, INNER_RING_SIZE.y, hash[1]);
            float outerRingSize = lerp(OUTER_RING_SIZE.x, OUTER_RING_SIZE.y, hash[0]);
            outerRingSize = outerRingSize - innerRingSize < MIN_RING_THICKNESS ? 0 : outerRingSize;

            float outerShape = SDF.Functions.Sphere(localPos, radius * outerRingSize);
            SDF.FieldOut ringOuter = SDF.Functions.Intersection(SDF.FieldOut(plane), SDF.FieldOut(outerShape));
            float innerShape = SDF.Functions.Sphere(localPos, radius * innerRingSize);

            float ringDist = SDF.Functions.Subtraction(SDF.FieldOut(innerShape), ringOuter).SignedDist;
            return SDF.FieldOut(ringDist, RING_MAT);
        }

        public float3 GetColor<TPlanetNoiseReader : IPlanetNoiseReader>(float3 position, 
            float material,
            TPlanetNoiseReader noiseReader)
        {
            int index = (int)round(material);

            if (index == 0)
            {
                return GetPlanetColor<TPlanetNoiseReader>(position, noiseReader);
            }
            else
            {
                return GetRingColor<TPlanetNoiseReader>(position, noiseReader);
            }
        }

        private float3 GetPlanetColor<TPlanetNoiseReader : IPlanetNoiseReader>(float3 position, 
            TPlanetNoiseReader noiseReader)
        {
            float3 rotatedCoord = mul(basis, position - center);
            float3 scaledCoord = (rotatedCoord * invScale) * PLANET_NOISE_SCALE;

            int noiseIndex = int(hash[0] * 2349);
            float noise = noiseReader.Read(scaledCoord + hash[1], noiseIndex);
            float3 color = Color.Sample4ColorGrad(colors, noise).rgb;

            return color;
        }

        private float3 GetRingColor<TPlanetNoiseReader : IPlanetNoiseReader>(float3 position, 
            TPlanetNoiseReader noiseReader)
        {
            float3 localPos =  abs(position - center) * invScale; 
            float gradient = length(localPos) * RING_NOISE_SCALE;

            int noiseIndex = int(hash[2] * 4930);
            float noise = noiseReader.Read(float3(gradient + hash[3]), noiseIndex);
            float3 color = Color.Sample4ColorGrad(colors, noise).rgb;

            return color * (hash[4] * 0.5f + 0.5f);
        }
    };
}