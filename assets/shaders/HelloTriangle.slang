// static float2 positions[3] = float2[](
//     float2(0.0, -0.5),
//     float2(0.5, 0.5),
//     float2(-0.5, 0.5)
// );

// static float3 colors[3] = float3[](
//     float3(1.0, 0.0, 0.0),
//     float3(0.0, 1.0, 0.0),
//     float3(0.0, 0.0, 1.0)
// );

static float2 ASPECT = float2(800, 600);

struct MeshData
{
    float2 positionOS;
    float3 color;
};

struct VertexData 
{
    float4 positionCS : SV_Position;
    float3 color;
};

struct UniformData
{
    float4x4 objToWorldMat;
    float4x4 worldToViewMat;
    float4x4 viewToClipMat;
};

ConstantBuffer<UniformData> _UniformBuffer;

[shader("vertex")]
VertexData HelloTriangleVert(MeshData input)
{
    VertexData output;
    float4 positionWS = mul(_UniformBuffer.objToWorldMat, float4(input.positionOS, 1.0));
    float4 positionVS = mul(_UniformBuffer.worldToViewMat, positionWS);
    output.positionCS = mul(_UniformBuffer.viewToClipMat, positionVS);
    output.color = input.color;
    return output;
}

[shader("fragment")]
float4 HelloTriangleFrag(VertexData input) : SV_Target
{
    return float4(input.color, 1);
}