#ifndef BEER_MACROS_INCLUDED
#define BEER_MACROS_INCLUDED

// Mesh Data Macros
#define M_POSITION float3 positionOS
#define M_NORMAL float3 normalOS
#define M_TANGENT float3 tangentOS
#define M_UV float2 texCoord
#define M_COLOR float4 color

// Buffer Slots
#define BIND_ENGINE_GLOBALS    [[vk::binding(0, 0)]]
#define BIND_MATERIAL_DATA     [[vk::binding(0, 1)]]
#define BIND_MATERIAL_TEX(idx) [[vk::binding(idx, 1)]]

#endif