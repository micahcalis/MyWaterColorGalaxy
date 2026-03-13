module BeerLambert;

public struct BeerLambert
{
    private float3 albedo;
    private float3 worldPos;
    private float3 worldNormal;
    private float3 lightPos;

    public __init(float3 albedo, 
        float3 worldPos, 
        float3 worldNormal,
        float3 lightPos)
    {
        this.albedo = albedo;
        this.worldPos = worldPos;
        this.worldNormal = normalize(worldNormal);
        this.lightPos = lightPos;
    }

    public float3 GetLambert()
    {
        float3 lightDir = normalize(lightPos - worldPos);
        float NdotL = dot(lightDir, worldNormal) * 0.5 + 0.5;
        return albedo * saturate(NdotL);
    }
};