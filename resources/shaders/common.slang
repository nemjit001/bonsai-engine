module common;

public struct Camera
{
    public float4x4 viewProjectMatrix;
};

public struct RenderObject
{
    public uint objectId;
};

public struct ObjectTransform
{
    public float4x4 modelMatrix;
    public float4x4 normalMatrix;
};

public struct DrawParams
{
    public uint cameraId;
    public uint objectId;
};
