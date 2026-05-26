#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Blit.BlitUtils;
import Galaxy.WatercolorFunctions;
import Deferred.DeferredUtils;
import Core.Beer3D;
import Math.PartialDerivative;
import Galaxy.WatercolorLit;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _TremorOffsetIntensity;
    float _DepthFadeThreshold;
    float2 _DynamicOffset;
    float _PaperDistortionIntensity;
    float _PaperTiling;
    float _GranulationIntensity;
};

BIND_BLIT_SRC(1)
BIND_MATERIAL_TEX(2) Sampler2D<float4> _NormalsOffset;
BIND_MATERIAL_TEX(3) Sampler2D<float4> _WatercolorData;
BIND_MATERIAL_TEX(4) Sampler2D<float> _DepthBuffer;
BIND_MATERIAL_TEX(5) Sampler2D<float4> _BlurredColor;
BIND_MATERIAL_TEX(6) Sampler2D<float4> _PaperTex;

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
        int width; int height; int levels;
        _PaperTex.GetDimensions(0, width, height, levels);
        float2 scale = (_ScreenParams.xy / float2(width, height)) * _PaperTiling;

        float4 sample = _PaperTex.Sample(frac((uv + _DynamicOffset)) * scale + offset);
        WC.PaperData data;
        data.Height = sample.b;
        data.Derivative = Math.PartialDerivative(sample.r, sample.g);
        data.Derivative.Uncompress();
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
    
    float2 tremorOffset = tremorOffsetter.GetOffset();
    WC.PaperData data = wcSampler.SamplePaper(0);
    float2 distortionOffset = WC.GetPaperDistortionOffset(data.Derivative, _PaperDistortionIntensity, 1.0f);
    float2 uvOffset = tremorOffset + distortionOffset;

    WC.WatercolorParams params = wcSampler.SampleParams(uvOffset);

    WC.ColorBleeder<WatercolorProcessingSampler> colorBleeder = WC.ColorBleeder<WatercolorProcessingSampler>(
        wcSampler,
        params.ColorBleed);

    float3 bleededColor = colorBleeder.GetColor(uvOffset);

    WC.EdgeDarkener<WatercolorProcessingSampler> edgeDarkener = WC.EdgeDarkener<WatercolorProcessingSampler>(
        wcSampler,
        bleededColor,
        1.0f);

    float3 edgeDarkenedColor = edgeDarkener.ComputeEdgeDarkening(uvOffset);
    
    WC.PaperGranulator granulator = WC.PaperGranulator(data.Height,
        edgeDarkenedColor,
        _GranulationIntensity,
        1);

    float3 granulatedPigment = granulator.Granulate();
    
    float3 base = WC::WCRDFData::CANVAS_COLOR;
    float3 cangiante = 0.1f * ((1.0f - data.Height * 0.4f) * (base - granulatedPigment)) + granulatedPigment;
    float3 minColor = granulatedPigment * (1.0f - 0.5f);

     return float4(max(cangiante, minColor), 1);
}