module SphereTracing;

import SDF;

namespace SphereTracing
{
    static int MAX_STEPS = 50;
    static float DST_EPS = 0.01f;
    static float AABB_MARCH_PADDING = 0.05f;
    static float SHOULD_MARCH_SCALAR = 3.0f;
    static float BASE_MAX_LENGTH = 100.0f;
    static float MAX_LENGTH_BIAS = 1e-3f;

    public struct Ray
    {
        private float3 origin;
        private float3 direction;
        private float maxLength;

        public __init(float3 _origin,
            float3 _direction, 
            float _maxLength = BASE_MAX_LENGTH,
            float nearPlane = 0)
        {
            direction = normalize(_direction);
            origin = _origin + direction * nearPlane;
            maxLength = _maxLength;
        }

        [mutating]
        public void SetMaxLength(float _maxLength)
        {
            maxLength = _maxLength;
        }

        public float3 GetOrigin()
        {
            return origin;
        }

        public float3 GetDirection()
        {
            return direction;
        }

        public float GetMaxLength()
        {
            return maxLength;
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
            float3 directionRcp = rcp(ray.GetDirection());
            float3 minPoint = center - extents;
            float3 maxPoint = center + extents; 

            float3 t0 = (minPoint - ray.GetOrigin()) * directionRcp;
            float3 t1 = (maxPoint - ray.GetOrigin()) * directionRcp;

            float3 tMin = min(t0, t1);
            float3 tMax = max(t0, t1);

            float tNear = max(max(tMin.x, tMin.y), tMin.z);
            tNear = max(0.0f, tNear);
            float tFar = min(min(tMax.x, tMax.y), tMax.z);

            float3 intersect = ray.GetOrigin() + ray.GetDirection() * (tNear + 0.001f);

            float maxLength = max(0.0f, (tFar - tNear) - MAX_LENGTH_BIAS);

            return Ray(intersect, ray.GetDirection(), maxLength);
        }

        public bool ShouldMarch(float3 position, bool isFrontFace, float nearPlane)
        {
            bool isCameraInside = IsInside(position, nearPlane * SHOULD_MARCH_SCALAR);
            return isCameraInside != isFrontFace;
        }
    };

    public struct SphereVolume
    {
        private float3 center;
        private float radius;

        public __init(float3 _center,
            float _radius)
        {
            center = _center;
            radius = _radius;
        }

        public bool IsInside(float3 position,
            float padding = 0)
        {
            float3 diff = position - center;
            float rPad = radius + padding;
            return dot(diff, diff) <= (rPad * rPad);
        }

        public Ray AlignWithSphere(Ray ray)
        {
            float3 L = center - ray.GetOrigin();
            float tca = dot(L, ray.GetDirection());

            float d2 = dot(L, L) - (tca * tca);
            float r2 = radius * radius;

            if (d2 > r2)
            {
                return Ray(ray.GetOrigin(), ray.GetDirection(), 0.0f);
            }

            float thc = sqrt(r2 - d2);

            float t0 = tca - thc;
            float t1 = tca + thc;

            if (t1 < 0.0f)
            {
                return Ray(ray.GetOrigin(), ray.GetDirection(), 0.0f);
            }

            float tNear = max(0.0f, t0);
            float tFar = t1;

            float3 intersect = ray.GetOrigin() + ray.GetDirection() * (tNear + 0.001f);

            float maxLength = max(0.0f, (tFar - tNear) - MAX_LENGTH_BIAS);

            return Ray(intersect, ray.GetDirection(), maxLength);
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

        public __init(T _sdf, Ray _ray)
        {
            sdf = _sdf;
            ray = _ray;
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

                if (result.TotalDistance > ray.GetMaxLength())
                {
                    break;
                }
            }

            return result;
        }
    };
}