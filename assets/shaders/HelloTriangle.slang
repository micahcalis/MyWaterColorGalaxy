static float2 ASPECT = float2(800, 600);

struct MeshData
{
    float3 positionOS;
    float2 texCoord;
    float4 color;
};

struct VertexData 
{
    float4 positionCS : SV_Position;
    float2 texCoord;
    float4 color;
};

struct UniformData
{
    float4x4 objToWorldMat;
    float4x4 worldToViewMat;
    float4x4 viewToClipMat;
};

ConstantBuffer<UniformData> _UniformBuffer;

//$ Pass Opaque
//$ Vertex HelloTriangleVert
//$ Fragment HelloTriangleFrag
//$ Cull Back
//$ Blend On
//$ DepthWrite True

[shader("vertex")]
VertexData HelloTriangleVert(MeshData input)
{
    VertexData output;
    float4 positionWS = mul(_UniformBuffer.objToWorldMat, float4(input.positionOS, 1.0));
    float4 positionVS = mul(_UniformBuffer.worldToViewMat, positionWS);
    output.positionCS = mul(_UniformBuffer.viewToClipMat, positionVS);
    output.color = input.color;
    output.texCoord = input.texCoord;
    return output;
}

Sampler2D texture;

[shader("fragment")]
float4 HelloTriangleFrag(VertexData input) : SV_Target
{
    float4 sample = texture.Sample(input.texCoord);
    return sample;
}