module ColorSpace;

namespace Color
{
    public enum Space
    {
        RGB,
        HSV
    }

    public struct ColorTraveler
    {
        public float3 Color;
        private Space currentSpace;

        public __init(float3 rgb)
        {
            Color = rgb;
            currentSpace = Space.RGB;
        }

        [mutating]
        public void ToggleSpace()
        {
            if (currentSpace == Space.RGB)
            {
                RGBtoHSV();
                currentSpace = Space.HSV;
            }
            else
            {
                HSVtoRGB();
                currentSpace = Space.RGB;
            }
        }

        public Space GetSpace()
        {
            return currentSpace;
        }

        [mutating]
        private void RGBtoHSV()
        {
            float3 c = Color;
            float4 k = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
            float4 p = lerp(float4(c.bg, k.wz), float4(c.gb, k.xy), step(c.b, c.g));
            float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));

            float d = q.x - min(q.w, q.y);
            float e = 1.0e-10;

            Color = float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
        }

        [mutating]
        private void HSVtoRGB()
        {
            float3 c = Color;
            float4 k = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
            float3 p = abs(frac(c.xxx + k.xyz) * 6.0 - k.www);
            Color = c.z * lerp(k.xxx, saturate(p - k.xxx), c.y);
        }
    }
}