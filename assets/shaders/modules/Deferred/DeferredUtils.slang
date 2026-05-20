#include "Core/BeerMacros.slang"

import Core.BeerCore;

module DeferredUtils;

#define ALBEDO_TARGET     float4 albedo: SV_Target0
#define NORMAL_TARGET     float2 normal: SV_Target1
#define MAT_TARGET        float4 material: SV_Target2
#define EMISSION_TARGET   float3 emission: SV_Target3
#define DEPTH_TARGET      float depth : SV_Depth
#define COLOR_TARGET      float4 color: SV_Target0

namespace Deferred
{
    public struct FragData
    {
        public ALBEDO_TARGET;
        public NORMAL_TARGET;
        public MAT_TARGET;
        public EMISSION_TARGET;
        public DEPTH_TARGET;
    };

    public struct TransparentFragData
    {
        public COLOR_TARGET;
        public DEPTH_TARGET;
    }

    static int MAT_COUNT = 2;

    public struct DeferredShadeData
    {
        public float3 albedo;
        public int materialIndex;
        public float3 positionWS;
        public float3 normalWS;
        public float smoothness;
        public float metallic;
        public float3 emission;
        public float ambientOcclusion;

        public __init(float3 _albedo,
            int _materialIndex,
            float3 _positionWS,
            float3 _normalWS,
            float _smoothness,
            float _metallic,
            float3 _emission,
            float _ambientOcclusion)
        {
            albedo = _albedo;
            materialIndex = _materialIndex;
            positionWS = _positionWS;
            normalWS = _normalWS;
            smoothness = _smoothness;
            metallic = _metallic;
            emission = _emission;
            ambientOcclusion = _ambientOcclusion;
        }
    };

    float2 EncodeNormalOctahedron(float3 normal)
    {
        normal /= (abs(normal.x) + abs(normal.y) + abs(normal.z));
        
        float2 oct = normal.z >= 0.0 ? normal.xy : (1.0 - abs(normal.yx)) * sign(normal.xy);
        return oct * 0.5 + 0.5;
    }


    float3 DecodeNormalOctahedron(float2 encoded)
    {
        float2 f = encoded * 2.0 - 1.0;
        float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
        float t = max(-n.z, 0.0);
        n.x += n.x >= 0.0 ? -t : t;
        n.y += n.y >= 0.0 ? -t : t;

        return normalize(n);
    }

    public bool IsBackground(float depth)
    {
        return depth >= 1.0f;
    }

    public struct DeferredWriter
    {
        private DeferredShadeData data;

        public __init(DeferredShadeData _data)
        {
            data = _data;
        }

        public FragData Write()
        {
            FragData frag;
            frag.albedo = float4(data.albedo, (float)data.materialIndex / (float)(MAT_COUNT - 1));
            frag.normal = EncodeNormalOctahedron(data.normalWS);
            frag.material = float4(data.smoothness, data.metallic, data.ambientOcclusion, 0);
            frag.emission = data.emission;
            frag.depth = WorldPosToDepth(data.positionWS);
            return frag;
        }
    }

    public float WorldPosToDepth(float3 positionWS)
    {
        float4 hitClipPos = mul(_ViewProjMat, float4(positionWS, 1.0));
        return hitClipPos.z / hitClipPos.w;
    }

    public struct DeferredReader
    {
        private float4 albedoSample;
        private float2 normalSample;
        private float4 materialSample;
        private float3 emissionSample;

        public __init(float4 _albedoSample,
            float2 _normalSample,
            float4 _materialSample,
            float3 _emissionSample)
        {
            albedoSample = _albedoSample;
            normalSample = _normalSample;
            materialSample = _materialSample;
            emissionSample = _emissionSample;
        }

        public DeferredShadeData Read(float3 positionWS)
        {
            return DeferredShadeData(albedoSample.rgb,
                GetMaterialIndex(),
                positionWS,
                DecodeNormalOctahedron(normalSample),
                materialSample.r,
                materialSample.g,
                emissionSample,
                materialSample.b);
        }

        private int GetMaterialIndex()
        {
            return (int)round(albedoSample.a * (MAT_COUNT - 1));
        }
    }
}