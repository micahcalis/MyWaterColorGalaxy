#include "Core/BeerMacros.slang"

import Core.BeerCore;
import SDF;
import Noise.Hash;
import Math.BeerMath;
import Math.Gradient;
import Core.Samplers;
import Noise.Hash;
import Noise.VoronoiNoise;
import Noise.PerlinNoise;

module GalaxySDFs;

namespace Galaxy
{
    const static int[5] PLANET_SEEDS = { 240, 30, 324, 23, 68 };
    const static float PLANET_RING_THICKNESS = 0.01f;
    const static float2 OUTER_RING_SIZE = float2(0.5f, 2.0f); 
    const static float2 INNER_RING_SIZE = float2(1.15f, 1.4f); 

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
            radius = 0.48f;
    
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

    const static float3 SUN_NOISE_DIR_R = normalize(float3(0.6f, -0.4f, 0.2f));
    const static float3 SUN_NOISE_DIR_G = normalize(float3(-0.2f, 0.2f, -0.8f));
    const static float SUN_NOISE_SPEED = 0.05f;
    const static float SUN_NOISE_SCALE = 1.5f;

    public struct Sun : SDF.SDF
    {
        private float radius = 0.8f;
        private float3 center;
        private float scale;
        private float invScale;
        private float3 color;
        private float gradientIntensity;

        public __init(float3 _center,
            float _scale,
            float3 _color,
            float _gradientIntensity)
        {
            center = _center;
            scale = _scale;
            invScale = rcp(scale);
            color = _color;
            gradientIntensity = _gradientIntensity;
        }

        public SDF.FieldOut Evaluate(SDF.MarchData data)
        {
            float3 localPos = (data.Position - center) * invScale;
            SDF.FieldOut sunDist = Sun(localPos);
            sunDist.SignedDist *= scale;
            return sunDist;
        }

        private SDF.FieldOut Sun(float3 localPos)
        {
            return SDF.FieldOut(SDF.Functions.Sphere(localPos, radius));
        }

        public float3 GetSunColor<TSunNoiseSampler : ISampler3DFloat2>(float3 position,
            float3 normal,
            TSunNoiseSampler noiseSampler)
        {
            float fresnel = GetFresnel(position, normal);
            float noise = GetNoise<TSunNoiseSampler>(position, noiseSampler);

            return GetMixColor(fresnel + noise);
        }

        private float GetFresnel(float3 position,
            float3 normal)
        {
            float3 viewDirWS = GetViewDirectionWS(position);
            float fresnel = 1.0f - saturate(dot(-viewDirWS, normal));
            return fresnel * fresnel * fresnel * fresnel;
        }

        private float GetNoise<TSunNoiseSampler : ISampler3DFloat2>(float3 position,
            TSunNoiseSampler noiseSampler)
        {
            float3 localPos = (position - center) * invScale;

            float3 noiseOffsetR = _Time * SUN_NOISE_SPEED * SUN_NOISE_DIR_R;
            float noiseR = noiseSampler.Sample((localPos + noiseOffsetR) * SUN_NOISE_SCALE).r;

            float3 noiseOffsetG = _Time * SUN_NOISE_SPEED * SUN_NOISE_DIR_G;
            float noiseG = noiseSampler.Sample((localPos + noiseOffsetG) * SUN_NOISE_SCALE).g;

            return noiseR * noiseG;
        }

        private float3 GetMixColor(float gradient)
        {
            float inkThickness = (gradient * gradientIntensity) + 1.0f; 
            return pow(color.rgb, inkThickness);
        }
    }

    public float3 GetRandomPaletteColor(int asteroidID,
        int seed,
        float3[4] colors)
    {
        float hash = Noise.Functions.Hash11(asteroidID, seed + 48358);
        int index = clamp(int(hash * 4.0f), 0, 3); 
        return colors[index];
    }

    static const float CRATER_DIST_THRESHOLD = 0.21f;
    static const int NUM_COMPONENTS = 3;
    static const float CRATER_FREQ = 0.25f;
    static const float CRATER_RADIUS = 0.4f;

    public interface IAsteroidNoiseReader
    {
        public float Read(float3 uvw);
    }

    struct AsteroidComponent
    {
        float3 offset;
        float radius;
    }

    public struct Asteroid<TAsteroidNoiseReader : IAsteroidNoiseReader> : SDF.SDF
    {
        private TAsteroidNoiseReader noiseReader;
        private float3 center;
        private float scale;
        private float invScale;
        private float3 color;
        private int asteroidID;
        private int seed;
        private int craterSeed;
        private AsteroidComponent[NUM_COMPONENTS] components = (AsteroidComponent[NUM_COMPONENTS])0;

        public __init(TAsteroidNoiseReader noiseReader,
            float3 _center,
            float _scale,
            float3 _color,
            int _asteroidID,
            int _seed)
        {
            center = _center;
            scale = _scale;
            invScale = rcp(scale);
            color = _color;
            asteroidID = _asteroidID;
            seed = _seed;

            [unroll(NUM_COMPONENTS)]
            for (int i = 0; i < NUM_COMPONENTS; i++)
            {
                components[i].offset = Noise.Functions.Hash13(asteroidID, seed + 34553 + (i * 123)) - 0.5f;
                components[i].radius = lerp(0.2f, 0.75f, Noise.Functions.Hash11(asteroidID, seed + 2932 + (i * 77)));
            }

            craterSeed = int(Noise.Functions.Hash11(asteroidID, seed + 4033) * 30392);
        }
        
        public SDF.FieldOut Evaluate(SDF.MarchData data)
        {
            float3 localPos = (data.Position - center) * invScale;
            SDF.FieldOut asteroidDist = Asteroid(localPos);

            if (asteroidDist.SignedDist < CRATER_DIST_THRESHOLD)
            {
                SDF.FieldOut craterDist = Craters(localPos);
                craterDist.SignedDist = -craterDist.SignedDist;
                asteroidDist = SDF.Functions.SmoothMax(asteroidDist, craterDist, 0.05f);
            }

            asteroidDist.SignedDist *= scale;

            return asteroidDist;
        }

        public SDF.FieldOut Asteroid(float3 localPos)
        {
            return Base(localPos);
        }

        private SDF.FieldOut Base(float3 localPos)
        {
            SDF.FieldOut smoothMinOut = SDF.FieldOut(SDF.Functions.Sphere(
                    localPos - components[0].offset, 
                    components[0].radius));

            [unroll(NUM_COMPONENTS - 1)]
            for (int i = 1; i < NUM_COMPONENTS; i++)
            {
                SDF.FieldOut currentOut = SDF.FieldOut(SDF.Functions.Sphere(
                    localPos - components[i].offset,
                    components[i].radius));

                smoothMinOut = SDF.Functions.SmoothMin(smoothMinOut, currentOut, 0.1f);
            }

            return smoothMinOut;
        }

        private SDF.FieldOut Craters(float3 localPos)
        {
            float rawNoise = noiseReader.Read(localPos * CRATER_FREQ);
            float scaledNoise = rawNoise;

            float craterRadius = 0.4f; 
            float craterSphere = scaledNoise - craterRadius;

            return SDF.FieldOut(craterSphere);
        }

        public float3 GetColor(float3[4] colors)
        {
            return GetRandomPaletteColor(asteroidID,
                seed,
                colors);
        }
    }

    static const int NUM_GOO_SPHERES = 8;

    struct GooSphere
    {
        float3 radius;
        float scale;
        float speed;
        float3x3 basis;
        float3 color;
    }

    public struct SpaceGoo : SDF.SDF
    {
        private float3 center;
        private float scale;
        private float invScale;      
        private int seed;
        private GooSphere[NUM_GOO_SPHERES] gooSpheres = (GooSphere[NUM_GOO_SPHERES])0;
        private static float3 gooColor;

        public __init(float3 _center,
            float _scale,
            int _seed,
            float3[4] _colors)
        {
            center = _center;
            scale = _scale;
            invScale = rcp(_scale);
            seed = _seed;

            [unroll(NUM_GOO_SPHERES)]
            for (int i = 0; i < NUM_GOO_SPHERES; i++)
            {
                gooSpheres[i].radius = Noise.Functions.Hash11(i, seed + 64) * 0.65f;
                gooSpheres[i].scale = lerp(0.2f, 0.5f, Noise.Functions.Hash11(i, seed + 3221));
                gooSpheres[i].speed = lerp(-0.5f, 0.5f, Noise.Functions.Hash11(i, seed + 55));
                
                float3 normal = normalize(Noise.Functions.Hash13(i, seed + 454) - 0.5f);
                gooSpheres[i].basis = Math.GetOrthogonalBasis(normal);
                gooSpheres[i].color = _colors[i % 4];
            } 

            gooColor = _colors[0];
        }

        public SDF.FieldOut Evaluate(SDF.MarchData data)
        {
            float3 localPos = (data.Position - center) * invScale;
            SDF.FieldOut gooDist = Goo(localPos);
            gooDist.SignedDist *= scale;

            return gooDist;
        }

        private SDF.FieldOut Goo(float3 localPos)
        {
            SDF.FieldOut smoothMinOut = Sphere(localPos, gooSpheres[0]);
            gooColor = gooSpheres[0].color;

            [unroll(NUM_GOO_SPHERES - 1)]
            for (int i = 1; i < NUM_GOO_SPHERES; i++)
            {
                SDF.FieldOut currentSphere = Sphere(localPos, gooSpheres[i]);

                smoothMinOut.Material = 0.0f;
                smoothMinOut = SDF.Functions.SmoothMin(smoothMinOut, currentSphere, 0.05f);

                gooColor = lerp(gooColor, gooSpheres[i].color, smoothMinOut.Material);
            }

            return smoothMinOut;
        }

        private SDF.FieldOut Sphere(float3 localPos, GooSphere sphere)
        {
            float x; float y;
            sincos(_Time * sphere.speed, y, x);
            float3 circularPos = float3(x, y, 0) * sphere.radius;
            float3 rotatedPos = mul(sphere.basis, circularPos);

            return SDF.FieldOut(SDF.Functions.Sphere(localPos - rotatedPos, sphere.scale), 1.0f);
        }

        public float3 GetColor()
        {
            return gooColor;
        }
    }

    static const float VOID_MAT = 0.0f;
    static const float HALO_MAT = 1.0f;
    static const float BLHOLE_HORIZON_RADIUS = 0.35f;
    static const float BLHOLE_DISK_OUTER_RADIUS = 0.7f;
    static const float BLHOLE_DISK_THICKNESS = 0.01f;
    static const float LIGHT_RING_MIN = 0.45f;
    static const float LIGHT_RING_MAX = 0.55f;
    static const float3 LIGHT_COL = float3(1);
    static const float BLKHOLE_NOISE_SPEED = 0.1f;
    static const float BLKHOLE_NOISE_FREQ = 30.0f;
    static const float BLKHOLE_NOISE_INTENSITY = 0.25f;
    static const float BLKHOLE_STEP_GRAD_SCALE = 15.0f;
    static const float BLKHOLE_STEP_GRAD_SMOOTH = 0.3f;

    public struct BlackHole : SDF.SDF
    {
        private float3 center;
        private float scale;
        private float invScale;      
        private int seed;  
        private float3 normal;
        private float3x3 basis;
        private float3[4] colors;
        private static float testGradient;
        
        public __init(float3 _center,
            float _scale,
            int _seed,
            float3[4] _colors)
        {
            center = _center;
            scale = _scale;
            invScale = rcp(scale);
            seed = _seed;
            
            normal = normalize(Noise.Functions.Hash13(223, seed + 9322) - 0.5f);
            basis = Math.GetOrthogonalBasis(normal);
            colors = _colors;
        }

        public SDF.FieldOut Evaluate(SDF.MarchData data)
        {
            float3 localPos = (data.Position - center) * invScale;
            
            SDF.FieldOut finalDist = GetBlackHoleShape(localPos, data.Direction);
            
            finalDist.SignedDist *= scale;
            finalDist.SignedDist *= 0.8f;

            return finalDist;
        }

        private SDF.FieldOut GetBlackHoleShape(float3 localPos, float3 viewDir)
        {
            float3 rotatedPos = mul(basis, localPos);
            float voidDist = SDF.Functions.Sphere(localPos, BLHOLE_HORIZON_RADIUS);
            float diskDist = max(length(rotatedPos.xz) - BLHOLE_DISK_OUTER_RADIUS, abs(rotatedPos.y) - BLHOLE_DISK_THICKNESS);
            diskDist = max(diskDist, -(length(localPos) - (BLHOLE_HORIZON_RADIUS + 0.02f)));

            float haloRadius = BLHOLE_HORIZON_RADIUS + 0.06f; 
            float shellDist = abs(length(localPos) - haloRadius) - BLHOLE_DISK_THICKNESS;

            float depth = dot(localPos, viewDir);
            float backMask = smoothstep(-0.05f, 0.15f, depth);
            shellDist = lerp(100.0f, shellDist, backMask);

            SDF.FieldOut accretionShape = SDF.Functions.SmoothMin(SDF.FieldOut(diskDist), SDF.FieldOut(shellDist), 0.12f);

            testGradient = abs(accretionShape.SignedDist);

            if (voidDist < accretionShape.SignedDist)
            {
                return SDF.FieldOut(voidDist, VOID_MAT);
            }
            else
            {
                return SDF.FieldOut(accretionShape.SignedDist, HALO_MAT);
            }
        }

        public float3 GetColor(float3 position, float material)
        {
            if (material < 0.5f)
            {
                return GetVoidColor();
            }
            else
            {
                return GetHaloColor(position);
            }
        }

        private float3 GetHaloColor(float3 position)
        {
            float3 localPos = (position - center) * invScale;
            float distanceGrad = length(localPos);
            float3 rotatedPos = mul(basis, localPos);
            float angleGrad = ((atan2(rotatedPos.z, rotatedPos.x + 1e-6f) + PI) / TAU);
            angleGrad = frac(angleGrad + _Time * BLKHOLE_NOISE_SPEED);
            
            Noise.TileablePerlin perlin = Noise.TileablePerlin(seed, float2(distanceGrad, angleGrad));
            float noise = perlin.Compute(int2(BLKHOLE_NOISE_FREQ));

            float stepGrad = distanceGrad + noise * BLKHOLE_NOISE_INTENSITY;
            float stepGradient = Math.SmoothedSteps(stepGrad * BLKHOLE_STEP_GRAD_SCALE, BLKHOLE_STEP_GRAD_SMOOTH);

            int2 id = int2(floor(float2(stepGradient, angleGrad)));

            float hash0 = Noise.Functions.Hash11(id.x, seed);
            int colorID0 = clamp((int)(hash0 * 4.0f), 0, 3);
            float3 colorCurrent = colors[colorID0];

            float hash1 = Noise.Functions.Hash11(id.x + 1, seed);
            int colorID1 = clamp((int)(hash1 * 4.0f), 0, 3);
            float3 colorNext = colors[colorID1];

            float blendWeight = stepGradient - floor(stepGradient); 
            float3 gradientColor = lerp(colorCurrent, colorNext, blendWeight);

            float lightRing = smoothstep(LIGHT_RING_MAX, LIGHT_RING_MIN, distanceGrad);


            return lerp(gradientColor, LIGHT_COL, lightRing);
        }

        private float3 GetVoidColor()
        {
            return 0;
        }
    }
}