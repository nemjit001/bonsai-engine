module unlit;

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

[[vk::binding(0, 0)]] StructuredBuffer<Camera>          _cameras            : register(t0, space0);
[[vk::binding(1, 0)]] StructuredBuffer<RenderObject>    _renderObjects      : register(t1, space0);
[[vk::binding(2, 0)]] StructuredBuffer<ObjectTransform> _objectTransforms   : register(t2, space0);
[[vk::binding(3, 0)]] StructuredBuffer<SubMesh>         _meshes             : register(t3, space0);
[[vk::binding(4, 0)]] StructuredBuffer<Material>        _materials          : register(t4, space0);
[[vk::push_constant]] ConstantBuffer<DrawParams>        _drawParams         : register(b0, space1);

[shader("vertex")]
VertexOutput VSUnlitMain(VertexInput input)
{
    // Get camera/object properties
    Camera camera = _cameras[_drawParams.cameraId];
    RenderObject renderObject = _renderObjects[_drawParams.objectId];
    ObjectTransform objectTransform = _objectTransforms[renderObject.objectId];

    // Transform vertex data
    float4 position = mul(float4(input.position, 1.0), objectTransform.modelMatrix);
    float4 normal = mul(float4(input.normal, 0.0), objectTransform.normalMatrix);
    float4 tangent = mul(float4(input.tangent, 0.0), objectTransform.normalMatrix);

    // Output vertex data
    VertexOutput result = (VertexOutput)0;
    result.position = mul(position, camera.viewProjectMatrix);
    result.normal = normal;
    result.tangent = tangent;
    result.texCoords = input.texCoords;
    return result;
}

[shader("pixel")]
PixelOutput PSUnlitMain(VertexOutput input)
{
    // Get mesh/material properties
    SubMesh subMesh = _meshes[_drawParams.meshId];
    Material material = _materials[subMesh.materialId];

    // Output unlit pixel color
    PixelOutput result = (PixelOutput)0;
    result.color = float4(material.albedo, 1.0);
    return result;
}
