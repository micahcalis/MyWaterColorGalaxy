#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import Deferred.DeferredUtils;
import Galaxy.GalaxyInstancing;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float4 _SkyboxColor;
    float _GranulationNoiseIntensity;
    float _Wetness;
}

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

FRAG Deferred.SkyboxFragData GalaxySkyboxFrag(VertexData input)
{
    TransformData transform = GetModelTransform(input.instanceID);
    Deferred.SkyboxFragData fragData;
    fragData.color = float4(_SkyboxColor.rgb, 1);

    fragData.normal = Deferred.PackTransparentNormalOffsetData(Galaxy.GetTremorNoise(input.positionWS, GetTransformCenter(transform.objToWorldMat), 0) * 0.3f);

    fragData.waterColValues = Deferred.PackTransparentWatercolorData(Galaxy.GetDistortion(_Wetness), 
        _GranulationNoiseIntensity, 
        _Wetness);

    return fragData;
}
