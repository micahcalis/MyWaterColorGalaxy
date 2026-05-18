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

    public interface SDF2D
    {
        public float Evaluate(float2 position);
    }

    namespace Functions
    {
        // All credits to: Inigo Quilez
        // https://iquilezles.org/articles/distfunctions/

        public float Sphere(float3 pos, float radius)
        {
            return length(pos) - radius;
        }

        public float Box(float3 position, float3 size)
        {
            float3 q = abs(position) - size;
            return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
        }

        public float Torus(float3 position, float major, float minor)
        {
            float2 q = float2(length(position.xz) - major, position.y);
            return length(q) - minor;
        }

        public float OctaHedron(float3 position, float scale)
        {
            position = abs(position);
            return (position.x + position.y + position.z - scale) * 0.57735027;
        }

        public float Star2D(float2 position, float radius, int number, float sharpness)
        {
            float an = 3.14159265359 / float(number);
            float en = 3.14159265359 / sharpness;  // m is between 2 and n
            float2 acs = float2(cos(an), sin(an));
            float2 ecs = float2(cos(en), sin(en)); 

            float angle = atan2(position.x, position.y);
            
            float wrappedAngle = angle - (2.0 * an) * floor(angle / (2.0 * an));
            float bn = wrappedAngle - an;

            position = length(position) * float2(cos(bn), abs(sin(bn)));
            position -= radius * acs;
            position += ecs * clamp(-dot(position, ecs), 0.0, radius * acs.y / ecs.y);
            
            return length(position) * sign(position.x);
        }

        public float Ring2D(float2 position, float radius, float thickness)
        {
            float sphere = length(position) - radius;
            return abs(sphere) - thickness;
        }

        public float Union(float a, float b)
        {
            return min(a, b);
        }

        public float Subtraction(float a, float b)
        {
            return max(-a, b);
        }

        public float Intersection(float a, float b)
        {
            return max(a, b);
        }

        public float Xor(float a, float b)
        {
            return max(min(a, b), -max(a, b));
        }

        public float SmoothMin(float a, float b, float k) 
        {
            k *= 4.0f;
            float h = max(k - abs(a - b), 0.0f);
            return min(a, b) - h * h * 0.25 / k;
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

    public struct Star : SDF2D
    {
        private float2 center;
        private float radius;
        private int number;
        private float sharpness;

        public __init(float2 _center,
            float _radius,
            int _number,
            float _sharpness)
        {
            center = _center;
            radius = _radius;
            number = _number;
            sharpness = _sharpness;
        }

        public float Evaluate(float2 position)
        {
            return Functions.Star2D(position - center,
                radius,
                number,
                sharpness);
        }
    }

    public struct Ring : SDF2D
    {
        private float2 center;
        private float radius;
        private float thickness;

        public float Evaluate(float2 position)
        {
            return Functions.Ring2D(position - center,
                radius,
                thickness);
        }
    }
}