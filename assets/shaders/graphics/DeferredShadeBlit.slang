#include "BeerMacros.slang"

import BeerCore;
import Blit.BlitUtils;
import Deferred.DeferredUtils;
import BeerLit;

BIND_BLIT_SRC(1)
BIND_MATERIAL_TEX(2) Sampler2D<float2> _GBufferNormals;
BIND_MATERIAL_TEX(3) Sampler2D _GBufferMaterial;
BIND_MATERIAL_TEX(4) Sampler2D<float> _DepthBuffer;

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


FRAG float4 DeferredShadeFrag(VertexData input) : SV_Target
{
    float4 albedoSample = _BlitSource.Sample(input.uv);
    float2 normalSample = _GBufferNormals.Sample(input.uv);
    float4 materialSample = _GBufferMaterial.Sample(input.uv);

   Deferred.DeferredReader reader = Deferred.DeferredReader(albedoSample,
        normalSample,
        materialSample);

    float depth = _DepthBuffer.Sample(input.uv);
    float3 positionWS = UVDepthToWorld(input.uv, depth);

    Deferred.DeferredShadeData data = reader.Read(positionWS);


    Lit.BRDF brdf = Lit.BRDF(GetMainLight(positionWS),
        GetViewDirectionWS(positionWS),
        data.normalWS,
        data.albedo,
        data.smoothness,
        data.metallic);

    return float4(brdf.Shade(), 1);
}