#include "Core/BeerMacros.slang"

import Core.BeerCore;

module DeferredUtils;

#define ALBEDO_TARGET            float4 albedo: SV_Target0
#define NORMAL_OFFSET_TARGET     float4 normal: SV_Target1
#define MAT_TARGET               float4 material: SV_Target2
#define EMISSION_TARGET          float3 emission: SV_Target3
#define WATERCOL_TARGET           float4 waterColValues: SV_Target4

#define DEPTH_TARGET      float depth : SV_Depth
#define COLOR_TARGET      float4 color: SV_Target0

namespace Deferred
{
    public struct FragData
    {
        public ALBEDO_TARGET;
        public NORMAL_OFFSET_TARGET;
        public MAT_TARGET;
        public EMISSION_TARGET;
        public WATERCOL_TARGET;
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

    public struct DeferredWatercolorData
    {
        public float paperDistortion;
        public float paperGranulation;
        public float colorBleed;
        public float pigmentTurbulence;
        public float2 uvOffset;

        public __init(float _paperDistortion,
            float _paperGranulation,
            float _colorBleed,
            float _pigmentTurbulence,
            float2 _uvOffset)
        {
            paperDistortion = _paperDistortion;
            paperGranulation = _paperGranulation;
            colorBleed = _colorBleed;
            pigmentTurbulence = _pigmentTurbulence;
            uvOffset = _uvOffset;
        }
    }

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

    float2 EncodeUVOffset(float2 offset)
    {
        return offset * 0.5f + 0.5f;
    }

    float2 DecodeUVOffset(float2 encoded)
    {
        return (encoded - 0.5f) * 2.0f;
    }

    public bool IsBackground(float depth)
    {
        return depth >= 1.0f;
    }

    public struct DeferredWriter
    {
        private DeferredShadeData shadeData;
        private DeferredWatercolorData watercolorData;

        public __init(DeferredShadeData _shadeData,
            DeferredWatercolorData _watercolorData)
        {
            shadeData = _shadeData;
            watercolorData = _watercolorData;
        }

        public FragData Write()
        {
            FragData frag;
            WriteShade(frag);
            WriteWatercolor(frag);
            return frag;
        }

        private void WriteShade(inout FragData frag)
        {
            frag.albedo = float4(shadeData.albedo, (float)shadeData.materialIndex / (float)(MAT_COUNT - 1));
            frag.normal.rg = EncodeNormalOctahedron(shadeData.normalWS);
            frag.material = float4(shadeData.smoothness, shadeData.metallic, shadeData.ambientOcclusion, 0);
            frag.emission = shadeData.emission;
            frag.depth = WorldPosToDepth(shadeData.positionWS);
        }

        private void WriteWatercolor(inout FragData frag)
        {
            frag.waterColValues = float4(watercolorData.paperDistortion,
                watercolorData.paperGranulation,
                watercolorData.colorBleed,
                watercolorData.pigmentTurbulence);

            frag.normal.ba = EncodeUVOffset(watercolorData.uvOffset);
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
        private float4 normalSample;
        private float4 materialSample;
        private float3 emissionSample;
        private float4 watercolorSample;

        public __init(float4 _albedoSample,
            float4 _normalSample,
            float4 _materialSample,
            float3 _emissionSample,
            float4 _watercolorSample)
        {
            albedoSample = _albedoSample;
            normalSample = _normalSample;
            materialSample = _materialSample;
            emissionSample = _emissionSample;
            watercolorSample = _watercolorSample;
        }

        public DeferredShadeData ReadShade(float3 positionWS)
        {
            return DeferredShadeData(albedoSample.rgb,
                GetMaterialIndex(),
                positionWS,
                DecodeNormalOctahedron(normalSample.rg),
                materialSample.r,
                materialSample.g,
                emissionSample,
                materialSample.b);
        }

        public DeferredWatercolorData ReadWatercolor()
        {
            return DeferredWatercolorData(watercolorSample.r,
                watercolorSample.g,
                watercolorSample.b,
                watercolorSample.a,
                DecodeUVOffset(normalSample.ba));
        }

        private int GetMaterialIndex()
        {
            return (int)round(albedoSample.a * (MAT_COUNT - 1));
        }
    }
}