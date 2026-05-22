#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Blit.BlitUtils;
import Deferred.DeferredUtils;
import BeerLit;
import Galaxy.WatercolorLit;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _Cangiante;
    float _Dilution;
    float _DiluteArea;
    float _DarkIntensity;
    float _LightColorIntensity;
    float _TurbulenceIntensity;
}

BIND_BLIT_SRC(1)
BIND_MATERIAL_TEX(2) Sampler2D<float4> _GBufferNormals;
BIND_MATERIAL_TEX(3) Sampler2D _GBufferMaterial;
BIND_MATERIAL_TEX(4) Sampler2D<float3> _GBufferEmission;
BIND_MATERIAL_TEX(5) Sampler2D<float4> _GBufferWatercolor;
BIND_MATERIAL_TEX(6) Sampler2D<float> _DepthBuffer;

struct MeshData
{
};

struct VertexData
{
    float4 positionCS : SV_Position;
    float2 uv;
}

//$ Pass DeferredShade
//$ Vertex DeferredShadeVert
//$ Fragment DeferredShadeFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData DeferredShadeVert(MeshData input, uint vertexId : SV_VertexID)
{
    VertexData output;
    Blit.BlitTriangle blit = Blit.BlitTriangle(vertexId);
    blit.FillData(output.positionCS, output.uv);
    return output;
}

public struct DeferredSampler : Deferred.IDeferredSampler
{
    private float2 uv;

    public __init(float2 _uv)
    {
        uv = _uv;
    }

    public float4 SampleAlbedo(float2 offset)
    {
        return _BlitSource.Sample(uv + offset);
    }

    public float4 SampleNormalOffset(float2 offset)
    {
        return _GBufferNormals.Sample(uv + offset);
    }

    public float4 SampleMaterial(float2 offset)
    {
        return _GBufferMaterial.Sample(uv + offset);
    }

    public float3 SampleEmission(float2 offset)
    {
        return _GBufferEmission.Sample(uv + offset);
    }

    public float4 SampleWatercolor(float2 offset)
    {
        return _GBufferWatercolor.Sample(uv + offset);
    }
}

FRAG float4 DeferredShadeFrag(VertexData input) : SV_Target
{
   Deferred.DeferredReader<DeferredSampler> reader = Deferred.DeferredReader<DeferredSampler>(
        DeferredSampler(input.uv));

    float depth = _DepthBuffer.Sample(input.uv);

    if (Deferred.IsBackground(depth))
    {
        discard;
    }

    float3 positionWS = UVDepthToWorld(input.uv, depth);

    Deferred.DeferredShadeData shadeData = reader.ReadShade(positionWS);
    Deferred.DeferredWatercolorData watercolorData = reader.ReadWatercolor();

    float3 color = float3(1, 0, 1);

    if (shadeData.materialIndex == 0)
    {
        Lit.BRDF brdf = Lit.BRDF(GetMainLight(positionWS),
            GetViewDirectionWS(positionWS),
            shadeData.normalWS,
            shadeData.albedo,
            shadeData.smoothness,
            shadeData.metallic);

        color = brdf.Shade() + shadeData.emission;
    }
    if (shadeData.materialIndex == 1)
    {
        color = shadeData.albedo + shadeData.emission;
    }
    if (shadeData.materialIndex == 2)
    {
        WC.WCRDFData wcrdfData = WC.WCRDFData(_Cangiante,
            _Dilution,
            _DiluteArea,
            shadeData.smoothness,
            _DarkIntensity,
            _LightColorIntensity,
            (watercolorData.pigmentTurbulence - 0.5f) * _TurbulenceIntensity);

        WC.WCRDF wcrdf = WC.WCRDF(wcrdfData,
            shadeData.albedo,
            GetMainLight(positionWS),
            shadeData.normalWS,
            GetViewDirectionWS(positionWS));

        color = wcrdf.Shade() + shadeData.emission;
    }

    return float4(color, 1);
}