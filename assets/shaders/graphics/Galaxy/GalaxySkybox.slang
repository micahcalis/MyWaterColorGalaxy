#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import Galaxy.GalaxyInstancing;
import Noise.PerlinNoise;
import Galaxy.UniverseSky;
import Core.Samplers;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _SkyboxColor;
    float _GranulationNoiseIntensity;
    float _Wetness;
    float _DustOpacity;
    float _BleedThreshold;
    float4 _ColorA;
    float4 _ColorB;
    float4 _ColorC;
}

BIND_MATERIAL_TEX(4) SamplerCube<float3> _DustNoiseCubemap;
BIND_MATERIAL_TEX(5) SamplerCube<float3> _BlurredDustNoiseCubemapA;
BIND_MATERIAL_TEX(6) SamplerCube<float3> _BlurredDustNoiseCubemapB;


struct MeshData
{
    M_POSITION;
}

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
    uint instanceID;
}

//$ Pass Skybox
//$ Vertex GalaxySkyboxVert
//$ Fragment GalaxySkyboxFrag
//$ Cull Front
//$ Blend Off
//$ ZWrite Off
//$ ZMode Always

VERT VertexData GalaxySkyboxVert(MeshData input, INSTANCEID)
{
    GETMODELDATA
    VertexData output;
    output.positionWS = ObjToWorld(transform, input.positionOS);
    output.positionCS = WorldToClip(output.positionWS);
    output.instanceID = instanceID;
    return output;
}

public struct DustNoiseSampler : ISamplerCubeFloat3
{
    public float3 Sample(float3 dir)
    {
        return _DustNoiseCubemap.Sample(dir);
    }
}

public struct BlurredDustNoiseSamplerA : ISamplerCubeFloat3
{
        public float3 Sample(float3 dir)
    {
        return _BlurredDustNoiseCubemapA.Sample(dir);
    }
}

public struct BlurredDustNoiseSamplerB : ISamplerCubeFloat3
{
        public float3 Sample(float3 dir)
    {
        return _BlurredDustNoiseCubemapB.Sample(dir);
    }
}

public struct DustSampler : Galaxy.IDustSampler
{
    public float3 SampleBase(float3 dir)
    {
        return _DustNoiseCubemap.Sample(dir);
    }

    public float3 SampleBlurA(float3 dir)
    {
        return _BlurredDustNoiseCubemapA.Sample(dir);
    }
    

    public float3 SampleBlurB(float3 dir)
    {
        return _BlurredDustNoiseCubemapB.Sample(dir);
    }
}

FRAG Deferred.SkyboxFragData GalaxySkyboxFrag(VertexData input)
{
    TransformData transform = GetModelTransform(input.instanceID);
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);

    Galaxy.DustResolver<DustSampler> dustResolver = Galaxy.DustResolver<DustSampler>(
        DustSampler(),
        viewDirWS,
        { float4(_ColorA.rgb, _DustOpacity),
            float4(_ColorB.rgb, _DustOpacity),
            float4(_ColorC.rgb, _DustOpacity) },
        _BleedThreshold);

    float4 dustColor = dustResolver.ComputeDust();

    Deferred.SkyboxFragData fragData;
    fragData.color = float4(dustColor.rgb, 1.0f);

    fragData.normal = Deferred.PackTransparentNormalOffsetData(Galaxy.GetTremorNoise(input.positionWS, GetTransformCenter(transform.objToWorldMat), 0) * 0.3f);

    fragData.waterColValues = Deferred.PackTransparentWatercolorData(Galaxy.GetDistortion(_Wetness), 
        _GranulationNoiseIntensity, 
       0);

    return fragData;
}
