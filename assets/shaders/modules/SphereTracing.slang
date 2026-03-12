module SphereTracing;

namespace SphereTracing
{
    static int MAX_STEPS = 50;
    static float DST_EPS = 0.05f;

    public struct Sphere
    {
        private float3 center;
        private float radius;

        public __init(float3 _center, float _radius)
        {
            center = _center;
            radius = _radius;
        }

        public float Evaluate(float3 position)
        {
            return SdfSphere(position - center, radius);
        }

        private float SdfSphere(float3 pos, float radius)
        {
            return length(pos) - radius;
        }
    };

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

    public struct SphereTracer
    {
        private Sphere sphere;
        private Ray ray;
        private AABB boundingBox;

        public __init(Sphere _sphere, Ray _ray, AABB _boundingBox)
        {
            sphere = _sphere;
            ray = _ray;
            boundingBox = _boundingBox;
        }

        public bool Trace()
        {
            float3 startPos = ray.GetOrigin();
            bool hit = false;
            
            for (int i = 0; i < MAX_STEPS; i++)
            {
                float dist = sphere.Evaluate(startPos);

                if (dist < DST_EPS)
                {
                    hit = true;
                    break;
                }

                startPos += dist * ray.GetDirection();

                if (!boundingBox.IsInside(startPos))
                {
                    break;
                }
            }

            return hit;
        }
    };
}