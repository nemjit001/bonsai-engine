import common;

struct VertexInput
{
    float3 position     : POSITION0;
    float3 normal       : NORMAL0;
    float3 tangent      : TANGENT0;
    float2 texCoords    : TEXCOORD0;
};

struct VertexOutput
{
    float4 position     : SV_Position;
    float4 normal       : NORMAL0;
    float4 tangent      : TANGENT0;
    float2 texCoords    : TEXCOORD0;
};

struct PixelOutput
{
    float4 color : SV_Target0;
};

[[vk::binding(0, 0)]] ConstantBuffer<Camera> CameraCB : register(b0, space0);
[[vk::binding(0, 1)]] ConstantBuffer<ObjectTransform> ObjectTransformCB : register(b0, space1);

[shader("vertex")]
VertexOutput VSUnlitMain(VertexInput input)
{
    float4 position = mul(ObjectTransformCB.modelMatrix, float4(input.position, 1.0));
    float4 normal = mul(ObjectTransformCB.normalMatrix, float4(input.normal, 0.0));
    float4 tangent = mul(ObjectTransformCB.normalMatrix, float4(input.tangent, 0.0));

    VertexOutput result = (VertexOutput)0;
    result.position = mul(CameraCB.viewProjectMatrix, position);
    result.normal = normal;
    result.tangent = tangent;
    result.texCoords = input.texCoords;
    return result;
}

[shader("pixel")]
PixelOutput PSUnlitMain(VertexOutput input)
{
    PixelOutput result = (PixelOutput)0;
    result.color = float4(input.texCoords, 0.0, 1.0);
    return result;
}
