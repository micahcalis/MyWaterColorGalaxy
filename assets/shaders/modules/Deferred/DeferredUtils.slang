#include "BeerMacros.slang"

import BeerCore;

module DeferredUtils;

#define COLOR_TARGET      float4 color: SV_Target0;
#define DEPTH_TARGET      float depth : SV_Depth

namespace Deferred
{
    public struct FragData
    {
        public COLOR_TARGET;
        public DEPTH_TARGET;
    };

    public float WorldPosToDepth(float3 positionWS)
    {
        float4 hitClipPos = mul(_ViewProjMat, float4(positionWS, 1.0));
        return hitClipPos.z / hitClipPos.w;
    }
}