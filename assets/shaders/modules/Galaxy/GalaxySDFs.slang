#include "Core/BeerMacros.slang"

import Core.BeerCore;
import SDF;

module GalaxySDFs;

namespace Galaxy
{
    public struct Planet : SDF.SDF
    {
        private float3 center;
        private float scale;
        private float invScale;
        private float radius;

        public __init(float3 _center, 
            float _scale)
        {
            center = _center;
            scale = _scale;
            invScale = rcp(scale);
            radius = 0.8f;
        }

        public float Evaluate(SDF.MarchData data)
        {
            float3 localPos = (data.Position - center) * invScale;
            return Planet(localPos) * scale;
        }

        private float Planet(float3 localPos)
        {
            float sphere = SDF.Functions.Sphere(localPos, radius);
            return sphere;
        }
    };
}