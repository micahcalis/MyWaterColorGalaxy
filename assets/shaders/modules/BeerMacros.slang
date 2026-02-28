#ifndef BEER_MACROS_INCLUDED
#define BEER_MACROS_INCLUDED

// Mesh Data Macros
#define M_POSITION float3 positionOS
#define M_NORMAL float3 normalOS
#define M_TANGENT float3 tangentOS
#define M_UV float2 texCoord
#define M_COLOR float4 color

// Buffer Slots
#define SLOT_ENGINE_GLOBALS register(b0, space0)
#define SLOT_MATERIAL_DATA  register(b0, space1)

#endif