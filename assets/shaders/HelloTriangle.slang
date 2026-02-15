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

[shader("vertex")]
VertexData HelloTriangleVert(MeshData input)
{
    VertexData output;
    output.positionCS = float4(input.positionOS, 0.0, 1.0);
    output.color = input.color;
    return output;
}

[shader("fragment")]
float4 HelloTriangleFrag(VertexData input) : SV_Target
{
    float2 uv = input.positionCS.xy;
    return float4(input.color, 1);
}