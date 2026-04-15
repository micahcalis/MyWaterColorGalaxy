module BeerSkybox;

import Core.BeerCore;

namespace Skybox
{
    public struct Sky
    {
        private float base;
        private float topExp;
        private float botExp;

        public __init(float3 _viewDirectionWS, float _topExp, float _botExp)
        {
            base = _viewDirectionWS.y;
            topExp = _topExp;
            botExp = _botExp;
        }

        public float3 GetColor()
        {
            float verticalMask = step(base, 0);
            float3 topColor = BlendTop(GetTopGradient());
            float3 botColor = BlendBottom(GetBottomGradient());
            return botColor * verticalMask + topColor * (1 - verticalMask);
        }

        private float GetTopGradient()
        {
           return pow(saturate(base), topExp);
        }

        private float3 BlendTop(float gradient)
        {
            return lerp(_ShadowColor.rgb, _SkyColor.rgb, gradient);
        }

        private float GetBottomGradient()
        {
            return pow(abs(base), botExp);
        }

        private float3 BlendBottom(float gradient)
        {
            return lerp(_ShadowColor.rgb, float3(0), gradient);
        }
    };

    public struct Sun
    {
        float3 viewDirectionWS;
        float sunSize;

        public __init(float3 _viewDirectionWS,
            float _sunSize)
        {
            viewDirectionWS = _viewDirectionWS;
            sunSize = _sunSize;
        }

        public float3 GetColor()
        {
            float gradient = GetGradient();
            return gradient * _MainLightColor.rgb;
        }

        private float GetGradient()
        {
            float VdotP = dot(viewDirectionWS, normalize(_MainLightPos));
            return pow(saturate(VdotP), rcp(sunSize));
        }
    }
}