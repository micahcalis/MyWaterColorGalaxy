#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer2D;

module SpriteMesh;

namespace Sprite
{
    public struct MeshData
    {
        public float2 position2DOS;
        public float2 texCoord;
    }

    public struct VertData
    {
        public float4 positionCS : SV_Position;
        public float2 uv;
    }

    public float4 GetPositionCS(MeshData input)
    {
        return mul(_UIOrthoProjMat, float4(input.position2DOS, 0, 1));
    }

    public struct Sprite
    {
        private float2 uv;
        private Sampler2D spriteTex;
        private float2 scale = (float2)1;
        private float2 offset = (float2)0;
        private float4 tint = (float4)1;
        private float3 buttonColor;

        public __init(float2 _uv, 
            Sampler2D _spriteTex,
            ButtonState _buttonstate = ButtonState.Default)
        {
            uv = _uv;
            spriteTex = _spriteTex;
            buttonColor = GetButtonColor(_buttonstate);
        }

        [mutating]
        public void SetScaleOffset(float2 _scale, float2 _offset)
        {
            scale = _scale;
            offset = _offset;
        }

        [mutating]
        public void SetTint(float4 _tint)
        {
            tint = _tint;
        }

        public float4 GetColor()
        {
            float2 coord = (uv * scale) + offset;
            float4 sample = spriteTex.Sample(coord);
            sample = saturate(float4(sample.rgb + buttonColor, sample.a));
            sample.rgb += buttonColor;
            return sample * tint;
        }
    }

    public enum ButtonState : int
    {
        Default = 0,
        Hover = 1,
        Click = 2
    };

    public float3 GetButtonColor(ButtonState state)
    {
        switch (state)
        {
            case ButtonState.Default:
                return float3(0, 0, 0);

            case ButtonState.Hover:
                return float3(0.2, 0.2, 0.2);

            case ButtonState.Click:
                return float3(0.5, 0.5, 0.5);
        }
        
        return float3(0, 0, 0);
    }
}