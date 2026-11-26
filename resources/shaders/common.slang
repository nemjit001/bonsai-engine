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

public struct SubMesh
{
    public uint materialId;
};

public struct Material
{
    public float3 albedo;
    public float metallic;
    public float roughness;
};

public struct DrawParams
{
    public uint cameraId;
    public uint objectId;
    public uint meshId;
};

/// @brief Calculate the TBN matrix for a normal and tangent vector.
/// @param normal Normal vector.
/// @param tangent Tangent vector.
/// @return The output TBN matrix.
public float3x3 calculateTBN(float3 normal, float3 tangent)
{
    // Reorthogonalize Normal and Tangent
    float3 N = normalize(normal);
    float3 T = normalize(tangent + dot(tangent, N) * N);

    // Calculate Bitangent and output TBN matrix
    float3 B = cross(N, T);
    return float3x3(T, B, N);
}
