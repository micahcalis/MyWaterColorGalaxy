module BeerLit;

import Lights;

namespace Lit
{
    public struct BRDF
    {
        private Light light;
        private float3 normal;
        private float3 viewDirection;
        private float3 albedo;
        private float smoothness;
        private float metallic;

        public __init(Light _light,
            float3 _viewDirection,
            float3 _normal,
            float3 _albedo,
            float _smoothness,
            float _metallic)
        {
            light = _light;
            normal = normalize(_normal);
            viewDirection = normalize(_viewDirection);
            albedo = _albedo;
            smoothness = _smoothness;
            metallic = _metallic;
        }

        public float3 Shade()
        {
            float3 lambert = Lambert();
            float3 metallic = Metallic(lambert);
            float3 specular = Specular();
            float3 fresnel = Fresnel();

            return metallic + specular + fresnel;
        }

        private float3 Lambert()
        {
            float NdotL = saturate(dot(normal, light.GetDirection()));
            float3 directDiffuse = albedo * NdotL * light.GetLightColor();
            float3 indirectDiffuse = albedo * light.GetSkyColor();
            float3 radiance = directDiffuse + indirectDiffuse * 0.5;

            return radiance;
        }

        private float3 Metallic(float3 lambert)
        {
            float roughness = 1 - metallic;
            float3 base = lambert * roughness;
            float3 specCol = light.GetSkyColor() * metallic;
            float fresnel = dot(viewDirection, normal);
            float3 schlik = (1 - saturate(fresnel)) * albedo * metallic * 0.1;
            return base + specCol * schlik;
        }

        private float3 Specular()
        {
            float3 halfVec = normalize(light.GetDirection() + -viewDirection);
            float NdotH = saturate(dot(normal, halfVec));
            float LdotH = saturate(dot(light.GetDirection(), halfVec));

            float roughness = 1.0 - min(smoothness, 0.99);
            float roughness2 = roughness * roughness;

            float d = NdotH * NdotH * (roughness2 - 1.0) + 1.00001f;
            float denominator = (d * d) * max(0.1, LdotH * LdotH);

            float specularTerm = roughness2 / denominator * 0.03;
            return light.GetLightColor() * specularTerm * smoothness;
        }

        private float3 Fresnel()
        {
            float NdotV = saturate(dot(-viewDirection, normal));
            float fresnelPower = lerp(4, 2, smoothness);
            float fresnelIntensity = smoothness * 0.3;
            float fresnel = pow(1 - NdotV, fresnelPower) * fresnelIntensity;
            return fresnel * light.GetSkyColor(); 
        }
    };
}