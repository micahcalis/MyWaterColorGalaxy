module Effects;

namespace Math
{
    // From Unity: https://docs.unity3d.com/Packages/com.unity.shadergraph@6.9/manual/Spherize-Node.html 
    public float2 SpherizeUV(float2 uv, float2 center, float strength, float2 offset)
    {
        float2 delta = uv - center;
        float delta2 = dot(delta.xy, delta.xy);
        float delta4 = delta2 * delta2;
        float2 delta_offset = delta4 * strength;
        return uv + delta * delta_offset + offset;
    }
}