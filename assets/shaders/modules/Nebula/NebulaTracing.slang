#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Nebula.NebulaUtlities;
import SphereTracing;
import Core.Beer3D;
import SDF;
import Math.BeerMath;
import Lights;

module NebulaTracing;

namespace Nebula
{
    #define MAX_NEBULA_TILES 31

    public uint GetTileId(float2 uv, float2 tileDimensions)
    {
        float2 safeDims = max(tileDimensions, float2(1.0f, 1.0f)); 
            
        uint2 tileExactRes = (uint2)safeDims;
        uint2 tileCoord = uint2(uv * safeDims);
        tileCoord = min(tileCoord, tileExactRes - 1);

        uint tileId = FlattenTileId(tileCoord, tileExactRes);
        uint maxTileID = (uint)(safeDims.x * safeDims.y) - 1;
        
        return min(tileId, maxTileID);
    }

    public bool ShouldTraceTile(NebulaTile tile)
    {
        return tile.NebulaCount > 0;
    }

    public SphereTracing.Ray GetRay(float2 uv, 
        float depth,
        float maxLength)
    {
        float3 pixelPosition = UVDepthToWorld(uv, depth);
        float3 cameraVec = pixelPosition - _CameraPos;
        float exactDist = length(cameraVec);
        float3 viewDirWS = cameraVec / exactDist;

        return SphereTracing.Ray(_CameraPos, viewDirWS, min(exactDist, maxLength));
    }

    // from: https://www.youtube.com/watch?v=Qj_tK_mdRcA
    public float HenyeyGreenstein(float g, float costh)
    {
        return (1.0 / (4.0 * PI)) * ((1.0 - g * g) / pow(abs(1.0 + g * g - 2.0 * g * costh), 1.5));
    }

    public float GetPhase(float g, float k, float3 viewDirWS, float3 lightDirWS)
    {
        float costH = dot(viewDirWS, lightDirWS);
        float phaseFront = HenyeyGreenstein(g, costH);
        float phaseBack = HenyeyGreenstein(-g, costH);
        return lerp(phaseBack, phaseFront, k);
    }

    public struct NebulaMaterial
    {
        public float4 ColorA;
        public float4 ColorB;
        public float4 ColorC;
        public float4 ColorD;
        public uint Seed;
    }

    const static float3 SHAPE_WEIGHTS = float3(0.625f, 0.25f, 0.125f);
    const static float3 DETAIL_WEIGHTS = float3(0.625f, 0.25f, 0.125f);

    
    public struct TraceInput
    {
        public uint MaxSteps;
        public float StepSize;
        public float Coverage;
        public float DetailIntensity;
        public float ShapeAbsorption; 
        public float LightAbsorption;
        public float LightMultiplier;
        public float Phase;

        public __init(uint _maxSteps,
            float _stepSize,
            float _coverage,
            float _detailIntensity,
            float _shapeAbsorption,
            float _lightAbsorption,
            float _lightMultiplier,
            float _henyeyG,
            float _henyeyK,
            float3 _viewDirWS,
            float3 _lightDirWS)
        {
            MaxSteps = _maxSteps;
            StepSize = _stepSize;
            Coverage = _coverage;
            DetailIntensity = _detailIntensity;
            ShapeAbsorption = _shapeAbsorption;
            LightAbsorption = _lightAbsorption;
            LightMultiplier = _lightMultiplier;
            float rawPhase = GetPhase(_henyeyG, _henyeyK, _viewDirWS, _lightDirWS);
            Phase = min(rawPhase, 1.0f);
        }
    }

    public struct TraceResult
    {
        public float Transmittance;
        public float3 Color;
        public float3 Position;
        public float TotalDistance;
        public bool Hit;

        public __init(float3 startPos)
        {
            Transmittance = 1;
            Color = (float3)0;
            TotalDistance = 0;
            Position = startPos;
            Hit = false;
        }
    }

    public interface INebulaNoiseSampler
    {
        public float4 GetShape(float3 uvw, float3 offset);
        public float4 GetDetail(float3 uvw, float3 offset);
        public float3 GetDistortion(float3 uvw, float3 offset);

        public float GetShapeDensity(float3 uvw,
            float3 offset,
            TraceInput input)
        {
            float4 sample = GetShape(uvw, offset);
            float densityBase = sample.r;

            float weightedDensity = dot(sample.gba, SHAPE_WEIGHTS);
            weightedDensity *= 1.0f - input.Coverage;

            return Math.Remap(densityBase, float2(weightedDensity, 1.0f), float2(0.0f, 1.0f));
        }

        public float GetDetailDensity(float3 uvw,
            float3 offset,
            TraceInput input)
        {
            float4 sample = GetDetail(uvw, offset);
            float densityBase = sample.r;

            float weightedDensity = dot(sample.gba, SHAPE_WEIGHTS);
            weightedDensity *= 1.0f - input.DetailIntensity;

            return Math.Remap(densityBase, float2(weightedDensity, 1.0f), float2(0.0f, 1.0f));
        }
    }


    public interface INebulaObjectGetter
    {
        public ObjectPair GetObject(uint index);
        public NebulaMaterial GetMaterial(uint index);
        public bool IsValid(uint index);
    }

    const static float DENS_EPS = 1e-3f;

    public struct NebulaContainer<TObjectGetter : INebulaObjectGetter,
        TNoiseSampler : INebulaNoiseSampler>
    {
        private NebulaTile tile;
        private TObjectGetter getter;
        private TNoiseSampler noiseSampler;

        public __init(NebulaTile _tile,
            TObjectGetter _getter,
            TNoiseSampler _noiseSampler)
        {
            tile = _tile;
            tile.NebulaCount = min(tile.NebulaCount, MAX_NEBULA_TILES);
            getter = _getter;
            noiseSampler = _noiseSampler;
        }

        public float GetSignedDistance(float3 position)
        {
            float minDist = 1e+7f;

            for (int i = 0; i < tile.NebulaCount; i++)
            {
                uint index = tile.NebulaIndices[i];
            
                if (!getter.IsValid(index)) 
                    continue; 

                ObjectPair object = getter.GetObject(index);
                float sphereDist = SDF.Functions.Sphere(position - object.Position, object.Scale);
                minDist = min(minDist, sphereDist);
            }

            return minDist;
        }

        public float GetTotalDensity(float3 position,
            float3 offset,
            TraceInput input,
            out bool insideVolumes)
        {
            float totalDensity = 0;

            for (int i = 0; i < tile.NebulaCount; i++)
            {
                uint index = tile.NebulaIndices[i];
            
                if (!getter.IsValid(index)) 
                    continue; 

                ObjectPair object = getter.GetObject(index);
                float sphereDist = SDF.Functions.Sphere(position - object.Position, object.Scale);

                if (sphereDist > 0)
                    continue;

                insideVolumes = true;

                totalDensity += GetObjDensity(object,
                    position,
                    offset,
                    input);
            }

            return totalDensity;
        }

        inline private float GetObjDensity(ObjectPair object,
            float3 position,
            float3 offset,
            TraceInput input)
        {
            float3 uvw = position - object.Position;

            float density = noiseSampler.GetShapeDensity(uvw,
                offset,
                input);

            density *= GetSphereGrad(uvw, object.Scale);

            if (density > DENS_EPS)
            {
                density *= noiseSampler.GetDetailDensity(uvw,
                    offset,
                    input);
            }

            return density;
        }

        public ObjectPair GetClosestObject(float3 position)
        {
            ObjectPair closestObject = ObjectPair(1e+8f, 1e-8f);
            float minDist = 1e+8f;

            for (int i = 0; i < tile.NebulaCount; i++)
            {
                uint index = tile.NebulaIndices[i];

                if (!getter.IsValid(index))
                    continue;

                ObjectPair compareObject = getter.GetObject(index);
                float compareDist = SDF.Functions.Sphere(position - compareObject.Position, compareObject.Scale);

                if (compareDist < minDist)
                {
                    closestObject = compareObject;
                    minDist = compareDist;
                }
            }
            
            return closestObject;
        }

        public float GetObjLightDensity(ObjectPair object,
            float3 position,
            float3 offset,
            TraceInput input)
        {
            float3 uvw = position - object.Position;

            float density = noiseSampler.GetShapeDensity(uvw,
                offset,
                input);

            density *= GetSphereGrad(uvw, object.Scale);

            return density;
        }

        inline private static float GetSphereGrad(float3 centerPos,
            float scale)
        {
            float linearGrad = length(centerPos) / scale;
            return smoothstep(1.0f, 0.5f, linearGrad);
        }
    }

    const static float DST_EPS = 0.05f;
    const static float TRANS_THRESH = 0.01f;
    const static int LIGHT_MARCH_STEPS = 10;
    const static float LIGHT_MARCH_LENGTH = 300.0f;

    public struct Tracer<TObjectGetter: INebulaObjectGetter,
        TNoiseSampler : INebulaNoiseSampler>
    {
        private SphereTracing.Ray ray;
        private NebulaContainer<TObjectGetter, TNoiseSampler> container;
        private TraceInput input;

        public __init(SphereTracing.Ray _ray,
            NebulaContainer<TObjectGetter, TNoiseSampler> _container,
            TraceInput _input)
        {
            ray = _ray;
            container = _container;
            input = _input;
        }

        public TraceResult Trace(float stepScalar = 1.0f)
        {
            TraceResult result = TraceResult(ray.GetOrigin());
            
            [loop]
            for (int i = 0; i < input.MaxSteps; i++)
            {
                float signedDist = container.GetSignedDistance(result.Position);
                float eps = DST_EPS + (0.002f * result.TotalDistance);

                float marchStep = 0;

                if (signedDist < eps)
                {
                    result.Hit = true;

                    VolumetricRaymarch(result);

                    if (result.Transmittance < TRANS_THRESH)
                    {
                        break;
                    }

                    marchStep = input.StepSize; 
                }
                else
                {
                    marchStep = signedDist * stepScalar;
                }

                result.Position += marchStep * ray.GetDirection();
                result.TotalDistance += marchStep;

                if (result.TotalDistance > ray.GetMaxLength())
                {
                    break;
                }
            }

            return result;
        }

        private void VolumetricRaymarch(inout TraceResult result)
        {
            float3 distortionOffset = float3(0, 0, 0);

            [loop]
            for (int i = 0; i < input.MaxSteps; i++)
            {
                bool insideVolumes = false;
                
                float stepDensity = container.GetTotalDensity(
                    result.Position, 
                    distortionOffset, 
                    input, 
                    insideVolumes);

                if (!insideVolumes) 
                    break;

                if (stepDensity > DENS_EPS)
                {
                    float stepTransmittance = exp(-stepDensity * input.StepSize * input.ShapeAbsorption);
                    result.Transmittance *= stepTransmittance;

                    float3 lightColor = MarchLightColor(result.Position,
                        stepDensity);

                    result.Color += (result.Transmittance) * lightColor;
                }

                result.Position += ray.GetDirection() * input.StepSize;
                result.TotalDistance += input.StepSize;

                if (result.Transmittance < TRANS_THRESH || result.TotalDistance > ray.GetMaxLength())
                {
                    break; 
                }
            }
        }

        private float3 MarchLightColor(float3 origin,
            float shapeDensity)
        {
            float transmittance = 1;
            float previousDist = 0;

            Light light = GetMainLight(origin);
            ObjectPair lightObject = container.GetClosestObject(origin);

            [unroll(LIGHT_MARCH_STEPS)]
            for (int i = 0; i < LIGHT_MARCH_STEPS; i++)
            {
                float t = (float)(i + 1) / (float)LIGHT_MARCH_STEPS;
                float currentDist = pow(t, 1.5f) * LIGHT_MARCH_LENGTH;

                float stepSize = currentDist - previousDist;
                previousDist = currentDist;

                float3 position = origin + light.GetDirection() * currentDist;

                float lightDensity = container.GetObjLightDensity(lightObject,
                    position,
                    0,
                    input);

                transmittance *= CalculateLightTransmittance(lightDensity,
                    shapeDensity,
                    stepSize);
            }

            float powderMod = (1.0f - exp(-shapeDensity * 2.0f)) * input.Phase;

            return (transmittance * powderMod * input.LightMultiplier) * light.GetLightColor();
        }

        private float CalculateLightTransmittance(float lightDensity,
            float shapeDensity,
            float stepSize)
        {
            return exp(-lightDensity * stepSize * input.LightAbsorption);
        }
    };
}