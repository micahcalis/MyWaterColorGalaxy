#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Galaxy.WatercolorFunctions;
import Deferred.DeferredUtils;
import Core.Beer3D;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _TremorOffsetIntensity;
    float _DepthFadeThreshold;
};

BIND_BLIT_SRC(1)
BIND_MATERIAL_TEX(2) Sampler2D<float4> _NormalsOffset;
BIND_MATERIAL_TEX(3) Sampler2D<float4> _WatercolorData;
BIND_MATERIAL_TEX(4) Sampler2D<float> _DepthBuffer;
BIND_MATERIAL_TEX(5) Sampler2D<float4> _BlurredColor;

struct MeshData
{
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float2 uv;
}

//$ Pass WatercolorProcessing
//$ Vertex WatercolorProcessingVert
//$ Fragment WatercolorProcessingFrag
//$ Cull None
//$ Blend Off
//$ ZTest Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData WatercolorProcessingVert(MeshData input, uint vertexID : SV_VertexID)
{
    VertexData output;
    Blit.BlitTriangle blit = Blit.BlitTriangle(vertexID);
    blit.FillData(output.positionCS, output.uv);
    return output;
}

public struct WatercolorProcessingSampler : WC.IWatercolorProcessingSampler
{
    private float2 uv;

    public __init(float2 _uv)
    {
        uv = _uv;
    }

    public WC.NormalsOffset SampleNormalsOffset(float2 offset)
    {
        WC.NormalsOffset normalsOffset;
        float4 sample = _NormalsOffset.Sample(uv + offset);
        normalsOffset.Normal = Deferred.DecodeNormalOctahedron(sample.rg);
        normalsOffset.Offset = Deferred.DecodeUVOffset(sample.ba);
        return normalsOffset;
    }

    public WC.WatercolorParams SampleParams(float2 offset)
    {
        WC.WatercolorParams wcParams;
        float4 sample = _WatercolorData.Sample(uv + offset);
        wcParams.PaperDistortion = sample.r;
        wcParams.PaperGranulation = sample.g;
        wcParams.ColorBleed = sample.b;
        wcParams.PigmentTurbulence = sample.a;
        return wcParams;
    }

    public float4 SampleMain(float2 offset)
    {
        return _BlitSource.Sample(uv + offset);
    }

    public float4 SampleBlurred(float2 offset)
    {
        return _BlurredColor.Sample(uv + offset);
    }

    public WC.PaperData SamplePaper(float2 offset)
    {
        // TODO: le paper
        WC.PaperData data;
        data.Height = 0;
        data.Slope = 0;
        return data;
    }

    public float SampleDepth(float2 offset)
    {
        return _DepthBuffer.Sample(uv + offset);
    }
}

FRAG float4 WatercolorProcessingFrag(VertexData input) : SV_Target
{
    WatercolorProcessingSampler wcSampler = WatercolorProcessingSampler(input.uv);
    float depth = _DepthBuffer.Sample(input.uv);
    float3 positionWS = UVDepthToWorld(input.uv, depth);
    float3 viewDirWS = GetViewDirectionWS(positionWS);

    WC.TremorOffsetter<WatercolorProcessingSampler> tremorOffsetter = WC.TremorOffsetter<WatercolorProcessingSampler>(
        wcSampler,
        _TremorOffsetIntensity,
        _DepthFadeThreshold,
        viewDirWS);

    return wcSampler.SampleMain(tremorOffsetter.GetOffset()) + wcSampler.SampleBlurred(0) * 0.5f;
}