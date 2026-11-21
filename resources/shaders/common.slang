module common;

public struct Camera
{
    public float4x4 viewProject;
};

public struct ObjectTransform
{
    public float4x4 model;
    public float4x4 normal;
};
