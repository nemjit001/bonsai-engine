#ifndef FORWARD_UNLIT_HLSL
#define FORWARD_UNLIT_HLSL

// Default static mesh vertex layout.
struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION0;
    [[vk::location(1)]] float3 normal   : NORMAL0;
    [[vk::location(2)]] float3 tangent  : TANGENT0;
    [[vk::location(3)]] float3 color    : COLOR0;
    [[vk::location(4)]] float2 texCoord : TEXCOORD0;
}

// Default vertex shader output.
struct VertexOutput
{
    float4 position : SV_POSITION0;
    [[vk::location(0)]] float3 color    : COLOR0;
    [[vk::location(1)]] float2 texCoord : TEXCOORD0;
}

// Static mesh vertex shader
[shader("vertex")]
VertexOutput VSStaticMain(VertexInput input)
{
    float4 position = float4(input.position, 1.0);

    VertexOutput result;
    result.position = position;
    result.color = input.color;
    result.texCoord = input.texCoord;
}

// Unlit pixel shader
[shader("pixel")]
float4 PSUnlitMain(VertexOutput input) : SV_TARGET0
{
    // TODO(nemjit001): Sample mesh textures instead of relying on vertex colors
    return float4(input.color, 1.0);
}

#endif //FORWARD_UNLIT_HLSL
