struct VertexInput
{
    float3 position : POSITION0;
};

struct VertexOutput
{
    float4 position : SV_Position;
};

struct PixelOutput
{
    float4 color : SV_Target0;
};

[shader("vertex")]
VertexOutput VSMain(VertexInput input) {
    VertexOutput result = {0};
    result.position = float4(input.position, 1.0);
    return result;
}

[shader("pixel")]
PixelOutput PSMain(VertexOutput input) {
    PixelOutput result = {0};
    result.color = input.position;
    return result;
}

[shader("compute")]
[numthreads(1, 1, 1)]
void CSMain() {
    //
}
