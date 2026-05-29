#include "Core/BeerMacros.slang"

BIND_MATERIAL_DATA
public cbuffer InstanceGlobalData
{
    public float _Smoothness;
    public float _Wetness;
    public float _DepthBleedMin;
    public float _DepthBleedMax;
    public float _GranulationNoiseIntensity;
    public float _Hardness;
}