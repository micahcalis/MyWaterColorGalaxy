#include "Core/BeerMacros.slang"

import Core.BeerCore;

module Samplers;

public interface ISampler2DFloat4
{
    public float4 Sample(float2 uv);
}

public interface ISampler2DFloat3
{
    public float3 Sample(float2 uv);
}

public interface ISampler2DFloat2
{
    public float2 Sample(float2 uv);
}

public interface ISampler2DFloat
{
    public float Sample(float2 uv);
}

public interface ISampler3DFloat4
{
    public float4 Sample(float3 uvw);
}

public interface ISampler3DFloat3
{
    public float3 Sample(float3 uvw);
}

public interface ISampler3DFloat2
{
    public float2 Sample(float3 uvw);
}

public interface ISampler3DFloat
{
    public float Sample(float3 uvw);
}

public interface ISamplerCubeFloat4
{
    public float4 Sample(float3 dir);
}

public interface ISamplerCubeFloat3
{
    public float3 Sample(float3 dir);
}

public interface ISamplerCubeFloat2
{
    public float2 Sample(float3 dir);
}

public interface ISamplerCubeFloat
{
    public float Sample(float3 dir);
}