#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Lights;

module WatercolorLit;

namespace WC
{
    public struct WCRDFData
    {
        public static float3 CANVAS_COLOR = float3(1.0f, 1.0f, 0.85f);

        public float Cangiante;
        public float Dilution;
        public float DiluteArea;
        public float Smoothness;
        public float DarkIntensity;
        public float LightColorIntensity;
        public float Turbulence;

        public __init(float cangiante,
            float dilution,
            float diluteArea,
            float smoothness,
            float darkIntensity,
            float lightColorIntensity,
            float turbulence)
        {
            Cangiante = cangiante;
            Dilution = dilution;
            DiluteArea = diluteArea;
            Smoothness = smoothness;
            DarkIntensity = darkIntensity;
            LightColorIntensity = lightColorIntensity;
            Turbulence = turbulence;
        }
    }
    
    public struct WCRDF
    {
        private WCRDFData data;
        private float3 albedo;
        private Light light;
        private float3 normal;
        private float3 viewDirection;
        private float hardness = 0;

        public __init(WCRDFData _data,
            float3 _albedo,
            Light _light,
            float3 _normal,
            float3 _viewDirection,
            float _hardness)
        {
            data = _data;
            albedo = _albedo;
            light = _light;
            normal = _normal;
            viewDirection = _viewDirection;
            hardness = _hardness;
        }

        public float3 Shade()
        {
            float area = GetArea();
            float3 cangianteColor = GetCangianteColor(area);
            float3 specularColor = GetSpecularColor();

            float3 wcrdf = cangianteColor + specularColor;
            return lerp(wcrdf, WCRDFData::CANVAS_COLOR, hardness);
        }

        public float GetArea()
        {
            float NdotL = saturate(dot(normal, light.GetDirection()));
            float area = (NdotL + (data.DiluteArea - 1.0f)) / data.DiluteArea;
            return area + data.Turbulence;
        }

        public float3 GetCangianteColor(float area)
        {
            float3 pigment = saturate(albedo + data.Cangiante * area);
            float3 base = lerp(WCRDFData::CANVAS_COLOR, light.GetLightColor(), data.LightColorIntensity);
            float3 cangiante = data.Dilution * (area * (base - pigment)) + pigment;
            float3 minColor = albedo * (1.0f - data.DarkIntensity);
            return max(cangiante, minColor);
        }

        public float3 GetSpecularColor()
        {
            float3 halfVec = normalize(light.GetDirection() + -viewDirection);
            float NdotH = saturate(dot(normal, halfVec));
            
            float roughness = 1.0f - data.Smoothness;
            float roughness2 = roughness * roughness * roughness * roughness;

            float edge = lerp(0.99f, 1.0f, roughness2);
            float mask = smoothstep(edge - 1e-4f, edge, NdotH);
            float opacity = pow(data.Smoothness, 4) * 1.5f;

            return mask * light.GetLightColor();
        }
    }
}