#include "Core/BeerMacros.slang"

import Core.BeerCore;
import Core.Beer3D;
import BeerSkybox;

BIND_MATERIAL_DATA
cbuffer MaterialData
{
    float _GradientExponentTop;
    float _GradientExponentBot;
    float _SunSize;
};

struct MeshData
{
    M_POSITION;
};

struct VertexData
{
    float4 positionCS : SV_Position;
    float3 positionWS;
}

//$ Pass Skybox
//$ Vertex SkyboxVert
//$ Fragment SkyboxFrag
//$ Cull Front
//$ Blend On
//$ Zwrite Off
//$ ZMode Always

VERT VertexData SkyboxVert(MeshData input, INSTANCEID)
{
    GETMODELDATA
    VertexData output;
    float4 positionWS = mul(transform.objToWorldMat, float4(input.positionOS, 1));
    output.positionCS = mul(_ViewProjMat, positionWS);
    output.positionWS = positionWS.xyz;
    return output;
}

FRAG float4 SkyboxFrag(VertexData input) : SV_Target
{
    float3 viewDirWS = GetViewDirectionWS(input.positionWS);
    Skybox.Sky sky = Skybox.Sky(viewDirWS, _GradientExponentTop,  _GradientExponentBot); 
    Skybox.Sun sun = Skybox.Sun(viewDirWS, _SunSize);    
    
    return float4(sky.GetColor() + sun.GetColor(), 1);
}
