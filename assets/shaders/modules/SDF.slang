module SDF;

namespace SDF
{
    static float NORMAL_EPS = 0.001f;

    public interface SDF
    {
        float Evaluate(float3 position);

        float3 GetNormal(float3 position, float totalDistance)
        {
            float eps = NORMAL_EPS + totalDistance * NORMAL_EPS;

            float dx = Evaluate(position + float3(eps, 0, 0)) - Evaluate(position - float3(eps, 0, 0));
            float dy = Evaluate(position + float3(0, eps, 0)) - Evaluate(position - float3(0, eps, 0));
            float dz = Evaluate(position + float3(0, 0, eps)) - Evaluate(position - float3(0, 0, eps));
            
            return normalize(float3(dx, dy, dz));
        }
    };

    namespace Functions
    {
        // All credits to: Inigo Quilez
        // https://iquilezles.org/articles/distfunctions/

        float Sphere(float3 pos, float radius)
        {
            return length(pos) - radius;
        }

        float Box(float3 position, float3 size)
        {
            float3 q = abs(position) - size;
            return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
        }

        float Torus(float3 position, float major, float minor)
        {
            float2 q = float2(length(position.xz) - major, position.y);
            return length(q) - minor;
        }

        float OctaHedron(float3 position, float scale)
        {
            position = abs(position);
            return (position.x + position.y + position.z - scale) * 0.57735027;
        }
    }

    public struct Sphere : SDF
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
            return Functions.Sphere(position - center, radius);
        }
    }

    public struct Box : SDF
    {
        private float3 center;
        private float3 size;

        public __init(float3 _center, float3 _size)
        {
            center = _center;
            size = _size;
        }

        public float Evaluate(float3 position)
        {
            return Functions.Box(position - center, size);
        }
    }

    public struct Torus : SDF
    {
        private float3 center;
        private float major;
        private float minor;

        public __init(float3 _center, float _major, float _minor)
        {
            center = _center;
            major = _major;
            minor = _minor;
        }

        public float Evaluate(float3 position)
        {
            return Functions.Torus(position - center, major, minor);
        }
    }

    public struct OctaHedron : SDF
    {
        private float3 center;
        private float scale;

        public __init(float3 _center, float _scale)
        {
            center = _center;
            scale = _scale;
        }

        public float Evaluate(float3 position)
        {
            return Functions.OctaHedron(position - center, scale);
        }
    }
}