module fullscreen_quad;

struct VertexOutput
{
    float4 position : SV_Position;
    float2 screenUV : TEXCOORD0;
};

struct PixelOutput
{
    float4 color : SV_Target0;
};

[[vk::binding(0, 0)]] Sampler2D _screenTexture : register(s0, space0);

[shader("vertex")]
VertexOutput VSFullscreenQuadMain(uint vertexID : SV_VertexID)
{
    // Generate screen UVs and screen positions
    float2 screenUV = float2((vertexID << 1) & 2, vertexID & 2);
    float2 screenPositions = 2.0 * screenUV - 1.0;

    VertexOutput result = (VertexOutput)0;
    result.position = float4(screenPositions, 0, 1);
    result.screenUV = screenUV * float2(1, -1); // Flip V component to adhere to clip space reqs of Bonsai
    return result;
}

[shader("pixel")]
PixelOutput PSFullscreenQuadMain(VertexOutput input)
{
    PixelOutput result = (PixelOutput)0;
    result.color = _screenTexture.Sample(input.screenUV);
    return result;
}
