module SDF;

namespace SDF
{
    static float NORMAL_EPS = 0.001f;

    public struct MarchData
    {
        public float3 Position;
        public float TotalDistance;
        public float3 Direction;
        public float LocalDistance;
    }

    public struct FieldOut
    {
        public float SignedDist;
        public float Material;

        public __init(float _signedDist, float _material = 0)
        {
            SignedDist = _signedDist;
            Material = _material;
        }
    }

    public interface SDF
    {
        public FieldOut Evaluate(MarchData data);

        public float3 GetNormal(MarchData data)
        {
            float eps = NORMAL_EPS + data.TotalDistance * NORMAL_EPS;

            MarchData a = data;
            MarchData b = data;

            a.Position = data.Position + float3(eps, 0, 0);
            b.Position = data.Position - float3(eps, 0, 0);
            float dx = Evaluate(a).SignedDist - Evaluate(b).SignedDist;

            a.Position = data.Position + float3(0, eps, 0);
            b.Position = data.Position - float3(0, eps, 0);
            float dy = Evaluate(a).SignedDist - Evaluate(b).SignedDist;

            a.Position = data.Position + float3(0, 0, eps);
            b.Position = data.Position - float3(0, 0, eps);
            float dz = Evaluate(a).SignedDist - Evaluate(b).SignedDist;
            
            return normalize(float3(dx, dy, dz));
        }
    }

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

        public float Plane(float3 position, float3 normal, float height)
        {
            return dot(position, normal) + height;
        }

        public float Line(float3 position, float3 a, float3 b)
        {
            float3 p = position;
            float k = dot(p - a, b - a) / dot(b - a, b - a);
            return distance(p, lerp(a, b, saturate(k)));
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

        public FieldOut Union(FieldOut a, FieldOut b)
        {
            return (a.SignedDist < b.SignedDist) ? a : b;
        }

        public FieldOut Subtraction(FieldOut a, FieldOut b)
        {
            return FieldOut(
                max(-a.SignedDist, b.SignedDist),
                (-a.SignedDist > b.SignedDist) ? a.Material : b.Material
            );
        }

        public FieldOut Intersection(FieldOut a, FieldOut b)
        {
            return (a.SignedDist > b.SignedDist) ? a : b;
        }

        public FieldOut Xor(FieldOut a, FieldOut b)
        {
            float d1 = min(a.SignedDist, b.SignedDist);
            float d2 = -max(a.SignedDist, b.SignedDist);
            
            float m1 = (a.SignedDist < b.SignedDist) ? a.Material : b.Material;
            float m2 = (a.SignedDist > b.SignedDist) ? a.Material : b.Material;
            
            return FieldOut(
                max(d1, d2), 
                (d1 > d2) ? m1 : m2
            );
        }

        public FieldOut SmoothMin(FieldOut a, FieldOut b, float k) 
        {
            k *= 4.0f;
            float h = max(k - abs(a.SignedDist - b.SignedDist), 0.0f);
            
            float dist = min(a.SignedDist, b.SignedDist) - h * h * 0.25f / k;
            float blendFactor = clamp(0.5f + (b.SignedDist - a.SignedDist) / k, 0.0f, 1.0f);
            
            return FieldOut(
                dist, 
                lerp(b.Material, a.Material, blendFactor));
        }

        public FieldOut SmoothMax(FieldOut a, FieldOut b, float k) 
        {
            k *= 4.0f;
            float h = max(k - abs(a.SignedDist - b.SignedDist), 0.0f);
            
            float dist = max(a.SignedDist, b.SignedDist) + h * h * 0.25f / k;
            float blendFactor = clamp(0.5f + (a.SignedDist - b.SignedDist) / k, 0.0f, 1.0f);
            
            return FieldOut(
                dist, 
                lerp(b.Material, a.Material, blendFactor));
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

        public FieldOut Evaluate(MarchData data)
        {
            return FieldOut(Functions.Sphere(data.Position - center, radius));
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

        public FieldOut Evaluate(MarchData data)
        {
            return FieldOut(Functions.Box(data.Position, size));
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

        public FieldOut Evaluate(MarchData data)
        {
            return FieldOut(Functions.Torus(data.Position - center, major, minor));
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

        public FieldOut Evaluate(MarchData data)
        {
            return FieldOut(Functions.OctaHedron(data.Position - center, scale));
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