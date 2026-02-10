static float2 positions[3] = float2[](
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
);

static float3 colors[3] = float3[](
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
);

struct VertexData 
{
    float4 positionCS : SV_Position;
    float3 color;
};

[shader("vertex")]
VertexData HelloTriangleVert(uint vertexId : SV_VertexID)
{
    VertexData output;
    output.positionCS = float4(positions[vertexId], 0.0, 1.0);
    output.color = colors[vertexId];
    return output;
}

[shader("fragment")]
float4 HelloTriangleFrag(VertexData input) : SV_Target
{
    float2 uv = input.positionCS.xy;
    return float4(frac(uv / 100.0f), 0, 1);
}