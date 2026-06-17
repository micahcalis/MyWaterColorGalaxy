#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Nebula.NebulaUtlities;
import SphereTracing;
import Core.Beer3D;
import SDF;

module NebulaTracing;

namespace Nebula
{
    #define MAX_NEBULA_TILES 32

    public uint GetTileId(float2 uv, float2 tileDimensions)
    {
        uint2 tileExactRes = (uint2)tileDimensions;
        uint2 tileCoord = uint2(uv * tileDimensions);
        tileCoord = min(tileCoord, tileExactRes - 1);
        
        return FlattenTileId(tileCoord, tileExactRes);
}

    public bool ShouldTraceTile(NebulaTile tile)
    {
        return tile.NebulaCount > 0;
    }

    public SphereTracing.Ray GetRay(float2 uv, float maxLength)
    {
        float3 dirPosition = UVDepthToWorld(uv, 0.5f);
        float3 viewDirWS = GetViewDirectionWS(dirPosition);

        return SphereTracing.Ray(_CameraPos, viewDirWS, maxLength);
    }

    public struct NebulaMaterial
    {
        public float4 ColorA;
        public float4 ColorB;
        public float4 ColorC;
        public float4 ColorD;
        public uint Seed;
    }

    public interface INebulaObjectGetter
    {
        public ObjectPair GetObject(uint index);
        public NebulaMaterial GetMaterial(uint index);
    }

    public struct NebulaContainer<TObjectGetter : INebulaObjectGetter>
    {
        private NebulaTile tile;
        private TObjectGetter getter;

        public __init(NebulaTile _tile,
            TObjectGetter _getter)
        {
            tile = _tile;
            getter = _getter;
        }

        public float GetSignedDistance(float3 position)
        {
            float minDist = 1e+7f;

            for (int i = 0; i < tile.NebulaCount; i++)
            {
                ObjectPair object = getter.GetObject(tile.NebulaIndices[i]);
                float sphereDist = SDF.Functions.Sphere(position - object.Position, object.Scale);
                minDist = min(minDist, sphereDist);
            }

            return minDist;
        }
    }

    public struct TraceInput
    {
        public uint MaxSteps;
        public float StepSize;
    }

    public struct TraceResult
    {
        public float Density;
        public float4 Color;
        public float3 Position;
        public float TotalDistance;
        public bool Hit;

        public __init(float3 startPos)
        {
            Density = 0;
            Color = (float4)0;
            TotalDistance = 0;
            Position = startPos;
            Hit = false;
        }
    };

    static float DST_EPS = 0.5f;

    public struct Tracer<TObjectGetter: INebulaObjectGetter>
    {
        private SphereTracing.Ray ray;
        private NebulaContainer<TObjectGetter> container;
        private TraceInput input;

        public __init(SphereTracing.Ray _ray,
            NebulaContainer<TObjectGetter> _container,
            TraceInput _input)
        {
            ray = _ray;
            container = _container;
            input = _input;
        }

        public TraceResult Trace(float stepScalar = 1.0f)
        {
            TraceResult result = TraceResult(ray.GetOrigin());

            for (int i = 0; i < input.MaxSteps; i++)
            {
                float signedDist = container.GetSignedDistance(result.Position);
                float eps = DST_EPS + (0.002f * result.TotalDistance);

                float marchStep = 0;

                if (signedDist < eps)
                {
                    result.Hit = true;

                    VolumetricRaymarch(result);

                    if (result.Density >= 1.0f)
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
            result.Density = 2.0f;
        }
    };
}