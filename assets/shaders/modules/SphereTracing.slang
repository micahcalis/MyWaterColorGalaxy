module SphereTracing;

import SDF;

namespace SphereTracing
{
    static int MAX_STEPS = 50;
    static float DST_EPS = 0.01f;

    public struct Ray
    {
        private float3 origin;
        private float3 direction;

        public __init(float3 _origin, float3 _direction)
        {
            origin = _origin;
            direction = normalize(_direction);
        }

        public float3 GetOrigin()
        {
            return origin;
        }

        public float3 GetDirection()
        {
            return direction;
        }
    };

    public struct AABB
    {
        private float3 center;
        private float3 extents;

        public __init(float3 _center, float3 size)
        {
            center = _center;
            extents = size * 0.5f;
        }

        public bool IsInside(float3 position)
        {
            float3 dist = abs(position - center);
            return all(dist <= extents);
        }
    };

    public struct TraceResult
    {
        public bool Hit;
        public float TotalDistance;
        public int TotalSteps;
        public float3 Position;
        public float3 Normal;

        public __init(float3 startPos)
        {
            Hit = false;
            TotalDistance = 0;
            TotalSteps = 0;
            Position = startPos;
            Normal = 0;
        }
    };

        public struct SphereTracer<T : SDF.SDF>
    {
        private T sdf;
        private Ray ray;
        private AABB boundingBox;

        public __init(T _sdf, Ray _ray, AABB _boundingBox)
        {
            sdf = _sdf;
            ray = _ray;
            boundingBox = _boundingBox;
        }

        public TraceResult Trace()
        {
            TraceResult result = TraceResult(ray.GetOrigin());
            
            for (int i = 0; i < MAX_STEPS; i++)
            {
                float dist = sdf.Evaluate(result.Position);

                if (dist < DST_EPS)
                {
                    result.Hit = true;
                    result.TotalSteps = i;
                    result.Normal = sdf.GetNormal(result.Position, result.TotalDistance);
                    break;
                }

                result.Position += dist * ray.GetDirection();
                result.TotalDistance += dist;

                if (!boundingBox.IsInside(result.Position))
                {
                    break;
                }
            }

            return result;
        }
    };
}