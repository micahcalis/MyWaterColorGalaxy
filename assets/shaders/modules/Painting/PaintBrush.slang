#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Painting.Pigment;

module PaintBrush;

namespace Painting
{
    public interface IWaterWrite
    {
        public void Write(float water);
    }

    public struct Brush
    {
        private float radius;
        private float smoothness;
        private float intensity;
        private Painting.Pigment pigment;

        public __init(float _radius,
            float _smoothness,
            float _intensity,
            Painting.Pigment _pigment)
        {
            radius = _radius;
            smoothness = _smoothness;
            intensity = _intensity;
            pigment = _pigment;
        }

        public void PaintOnWater<TWaterWrite : IWaterWrite>(TWaterWrite writer,
            float brushDistance)
        {
            writer.Write(GetBrushScalar(brushDistance));
        }

        private float GetBrushScalar(float brushDistance)
        {
            float brushGrad = saturate(1 - (brushDistance / radius));
            brushGrad = pow(brushGrad, smoothness);
            float brushDelta = intensity * _DeltaTime;
            
            return brushGrad * brushDelta;
        }
    }
}