#ifndef BEER_MACROS_INCLUDED
#define BEER_MACROS_INCLUDED

// Mesh Data Macros
#define M_POSITION float3 positionOS
#define M_NORMAL float3 normalOS
#define M_TANGENT float3 tangentOS
#define M_UV float2 texCoord
#define M_COLOR float4 color

// Buffer Slots
#define BIND_ENGINE_GLOBALS       [[vk::binding(0, 0)]]
#define BIND_LIGHTING_GLOBALS     [[vk::binding(1, 0)]]
#define BIND_MATERIAL_DATA        [[vk::binding(0, 2)]]
#define BIND_MATERIAL_TEX(idx)    [[vk::binding(idx, 2)]]
#define BIND_MATERIAL_BUFFER(idx) [[vk::binding(idx, 2)]]
#define BIND_MODEL_PUSH           [[vk::push_constant]]
#define BIND_INSTANCING_BUFFER    [[vk::binding(0, 1)]]

// Function Shortcuts
#define VERT                      [shader("vertex")]
#define FRAG                      [shader("fragment")]
#define COMP                      [shader("compute")]

// Transform
#define INSTANCEID uint instanceID : SV_InstanceID
#define GETMODELDATA TransformData transform = GetModelTransform(instanceID);

// string utils
#define _BEER_STR(x) #x
#define BEER_STR(x) _BEER_STR(x)
#define BEER_CONCAT(a, b) a ## b

// Blit
#define BIND_BLIT_SRC(idx) [[vk::binding(idx, 2)]] Sampler2D _BlitSource;

// Math
#define PI 3.14159265359f
#define TAU 6.28318530717f
#define RAD2DEG 57.2957795131f
#define DEG2RAD 0.01745329251f
 
#endif