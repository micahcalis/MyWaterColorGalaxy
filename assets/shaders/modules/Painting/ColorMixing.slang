#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Math.BeerMath;
import ColorSpace;

module ColorMixing;

namespace Painting
{
    public struct Canvas
    {
        private float2 topRight;
        private float2 topLeft;
        private float2 botRight;
        private float2 botLeft;

        public __init(float2 _topRight,
            float2 _topLeft,
            float2 _botRight,
            float2 _botLeft)
        {
            topRight = _topRight;
            topLeft = _topLeft;
            botRight = _botRight;
            botLeft = _botLeft;
        }

        public bool InCanvas(float2 position)
        {

            return position.x >= botLeft.x && position.x < topRight.x &&
                position.y >= botLeft.y && position.y < topRight.y;
        }

        public float2 AlignToCanvas(float2 pixelPos)
        {
            return float2(Math.InverseLerp(botLeft.x, topRight.x, pixelPos.x),
                Math.InverseLerp(botLeft.y, topRight.y, pixelPos.y));
        }
    }

    public struct PaintPixel
    {
        public float4 Cmyk;
        public float Alpha;

        public __init(float3 _rgb, float _alpha)
        {
            Cmyk = Color.Functions.RGBtoCMYK(_rgb);
            Alpha = _alpha;
        }
    }

    public struct Mixer
    {
        private PaintPixel base;

        public __init(float3 baseRgb, float baseAlpha)
        {
            base = PaintPixel(baseRgb, baseAlpha);
        }

        [mutating]
        public void Mix(float3 addRgb, float addAlpha)
        {
            PaintPixel mix = PaintPixel(addRgb, addAlpha);

            float4 inkAdded = mix.Cmyk * mix.Alpha;
            base.Cmyk = base.Cmyk + (inkAdded * (1.0 - base.Cmyk));

            base.Alpha = saturate(base.Alpha + mix.Alpha * (1.0 - base.Alpha));
        }
        public float3 GetColor()
        {
            return Color.Functions.CMYKtoRGB(base.Cmyk);
        }

        public float GetAlpha()
        {
            return base.Alpha;
        }
    }
}