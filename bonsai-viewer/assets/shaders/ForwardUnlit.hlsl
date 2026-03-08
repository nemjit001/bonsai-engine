#ifndef FORWARD_UNLIT_HLSL
#define FORWARD_UNLIT_HLSL

struct DrawConstants
{
    //
};

struct CameraBuffer
{
    float4x4 ViewProjectMatrix;
};

struct ObjectTransformBuffer
{
    float4x4 ModelMatrix;
    float4x4 NormalMatrix;
};

struct MaterialBuffer
{
    float3 Albedo;
    float Metallic;
    float Rougness;
};

[[vk::push_constant]] ConstantBuffer<DrawConstants> DrawParams              : register(b0, space0);
[[vk::binding(0, 0)]] ConstantBuffer<CameraBuffer> Camera                   : register(b1, space0);
[[vk::binding(1, 0)]] ConstantBuffer<ObjectTransformBuffer> ObjectTransform : register(b2, space0);
[[vk::binding(2, 0)]] ConstantBuffer<MaterialBuffer> Material               : register(b3, space0);

// Default static mesh vertex layout.
struct VertexInput
{
    [[vk::location(0)]] float3 position : POSITION0;
    [[vk::location(1)]] float3 normal   : NORMAL0;
    [[vk::location(2)]] float3 tangent  : TANGENT0;
    [[vk::location(3)]] float3 color    : COLOR0;
    [[vk::location(4)]] float2 texCoord : TEXCOORD0;
};

// Default vertex shader output.
struct VertexOutput
{
    float4 position : SV_POSITION0;
    [[vk::location(0)]] float3 normal   : NORMAL0;
    [[vk::location(1)]] float3 tangent  : TANGENT0;
    [[vk::location(2)]] float3 color    : COLOR0;
    [[vk::location(3)]] float2 texCoord : TEXCOORD0;
};

// Forward shading pixel shader output.
struct PixelOutput
{
    float4 ColorTarget : SV_Target0;
};

// Static mesh vertex shader
[shader("vertex")]
VertexOutput VSStaticMain(VertexInput input)
{
    float4 position = mul(float4(input.position, 1.0), ObjectTransform.ModelMatrix);
    float4 normal = mul(float4(input.normal, 0.0), ObjectTransform.NormalMatrix);
    float4 tangent = mul(float4(input.tangent, 0.0), ObjectTransform.NormalMatrix);

    VertexOutput result;
    result.position = mul(position, Camera.ViewProjectMatrix);
    result.normal = normalize(normal.xyz);
    result.tangent = normalize(tangent.xyz);
    result.color = input.color;
    result.texCoord = input.texCoord;

    return result;
}

// Unlit pixel shader
[shader("pixel")]
PixelOutput PSUnlitMain(VertexOutput input)
{
    PixelOutput result = (PixelOutput)0;
    result.ColorTarget = float4(input.color, 1.0);

    return result;
}

#endif //FORWARD_UNLIT_HLSL
