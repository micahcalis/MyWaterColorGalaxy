module SphereTracing;

import SDF;

namespace SphereTracing
{
    static int MAX_STEPS = 50;
    static float DST_EPS = 0.01f;
    static float AABB_MARCH_PADDING = 0.05f;
    static float SHOULD_MARCH_SCALAR = 3.0f;

    public struct Ray
    {
        private float3 origin;
        private float3 direction;

        public __init(float3 _origin, float3 _direction, float nearPlane = 0)
        {
            direction = normalize(_direction);
            origin = _origin + direction * nearPlane;
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

        public bool IsInside(float3 position, float padding = 0)
        {
            float3 dist = abs(position - center);
            return all(dist <= extents + padding);
        }

        public Ray AlignWithBox(Ray ray)
        {
            if (IsInside(ray.GetOrigin()))
            {
                return ray;
            }

            float3 directionRcp = rcp(ray.GetDirection());
            float3 minPoint = center - extents;
            float3 maxPoint = center + extents; 

            float3 t0 = (minPoint - ray.GetOrigin()) * directionRcp;
            float3 t1 = (maxPoint - ray.GetOrigin()) * directionRcp;

            float3 tMin = min(t0, t1);
            float3 tMax = max(t0, t1);

            float tNear = max(max(tMin.x, tMin.y), tMin.z);
            tNear = max(0.0f, tNear);

            float3 intersect = ray.GetOrigin() + ray.GetDirection() * (tNear + 0.001f);
            return Ray(intersect, ray.GetDirection());
        }

        public bool ShouldMarch(float3 position, bool isFrontFace, float nearPlane)
        {
            bool isCameraInside = IsInside(position, nearPlane * SHOULD_MARCH_SCALAR);
            return isCameraInside != isFrontFace;
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

                if (!boundingBox.IsInside(result.Position, AABB_MARCH_PADDING))
                {
                    break;
                }
            }

            return result;
        }
    };
}