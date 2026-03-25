vec3 processVertexBiTangent()
{
    return normalize(mat3(object.normal_matrix) * in_bitangent);
}

vec3 processVertexTangent()
{
    return normalize(mat3(object.normal_matrix) * in_tangent);
}
