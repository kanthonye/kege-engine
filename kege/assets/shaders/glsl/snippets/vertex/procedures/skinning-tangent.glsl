vec3 processVertexBiTangent()
{
    return normalize(mat3(bone_transform) * in_bitangent);
}

vec3 processVertexTangent()
{
    return normalize(mat3(bone_transform) * in_tangent);
}
