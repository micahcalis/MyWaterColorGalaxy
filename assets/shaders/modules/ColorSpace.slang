module ColorSpace;

namespace Color
{
    public enum Space
    {
        RGB,
        HSV,
        CMYK
    }

    public struct ColorTraveler
    {
        public float4 Color;
        private Space currentSpace;

        public __init(float3 rgb)
        {
            Color = float4(rgb, 1);
            currentSpace = Space.RGB;
        }

        [mutating]
        public void ToggleSpaceHSV()
        {
            if (currentSpace == Space.RGB)
            {
                Color = float4(Functions.RGBtoHSV(Color.rgb), 1.0f);
                currentSpace = Space.HSV;
            }
            else
            {
                Color = float4(Functions.HSVtoRGB(Color.rgb), 1.0f);
                currentSpace = Space.RGB;
            }
        }

        public Space GetSpace()
        {
            return currentSpace;
        }
    }

    namespace Functions
    {
        public float3 RGBtoHSV(float3 rgb)
        {
            float3 c = rgb;
            float4 k = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
            float4 p = lerp(float4(c.bg, k.wz), float4(c.gb, k.xy), step(c.b, c.g));
            float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));

            float d = q.x - min(q.w, q.y);
            float e = 1.0e-10;

            return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
        }

        public float3 HSVtoRGB(float3 hsv)
        {
            float3 c = hsv;
            float4 k = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
            float3 p = abs(frac(c.xxx + k.xyz) * 6.0 - k.www);
            return c.z * lerp(k.xxx, saturate(p - k.xxx), c.y);
        }

        public float3 CMYKtoRGB(float4 cmyk)
        {
            float c = cmyk.x;
            float m = cmyk.y;
            float y = cmyk.z;
            float k = cmyk.w;

            float invK = 1.0 - k;
            float r = 1.0 - min(1.0, c * invK + k);
            float g = 1.0 - min(1.0, m * invK + k);
            float b = 1.0 - min(1.0, y * invK + k);

            return clamp(float3(r, g, b), 0.0, 1.0);
        }

        public float4 RGBtoCMYK(float3 rgb) 
        {
            float r = rgb.r;
            float g = rgb.g;
            float b = rgb.b;
            float k = min(1.0 - r, min(1.0 - g, 1.0 - b));
            float3 cmy = (float3)0;
            float invK = 1.0 - k;

            if (invK != 0.0) 
            {
                cmy.x = (1.0 - r - k) / invK;
                cmy.y = (1.0 - g - k) / invK;
                cmy.z = (1.0 - b - k) / invK;
            }

            return clamp(float4(cmy, k), 0.0, 1.0);
        }
    }
}