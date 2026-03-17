module Lights;

public struct Light
{
    private float3 position;
    private float3 direction;
    private float3 lightColor;
    private float3 shadowColor;
    private float3 skyColor;

    public __init(float3 _position,
        float3 shadePoint,
        float3 _lightColor,
        float3 _shadowColor,
        float3 _skyColor)
    {
        position = _position;
        direction = normalize(position - shadePoint);
        lightColor = _lightColor;
        shadowColor = _shadowColor;
        skyColor = _skyColor;
    }

    public float3 GetPosition()
    {
        return position;
    }

    public float3 GetDirection()
    {
        return direction;
    }

    public float3 GetLightColor()
    {
        return lightColor;
    }

    public float3 GetShadowColor()
    {
        return shadowColor;
    }

    public float3 GetSkyColor()
    {
        return skyColor;
    }
};