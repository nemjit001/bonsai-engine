module common;

public struct Camera
{
    public float4x4 viewProjectMatrix;
};

public struct ObjectTransform
{
    public float4x4 modelMatrix;
    public float4x4 normalMatrix;
};
