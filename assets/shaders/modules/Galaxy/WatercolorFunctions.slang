#include "Core/BeerMacros.slang"

import Core.BeerCore;

module WatercolorFunctions;

namespace WC
{
    public float ApplyBloat(float scale, float3 position, float bloatModifier)
    {
        float scaledDist = distance(position, _CameraPos);
        return scale + sqrt(scaledDist) * bloatModifier;
    }
}