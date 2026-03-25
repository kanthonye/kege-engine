layout(std140, set = SKELETAL_SET, binding = SKELETAL_BINDING) uniform SkeletalData
{
    mat4 bones[128];
}

mat4 bone_transform;

void calcInverseTransposeBoneMatrix()
{
    mat4 bone_transform = mat4(0.0);
    for (int i = 0; i < 4; i++)
    {
        int bone_index = in_bone_indices[i];
        float weight = in_bone_weights[i];

        if (bone_index >= 0 && weight > 0.0)
        {
            bone_transform += bones[bone_index] * weight;
        }
    }
}

vec3 processVertexPosition()
{
    return vec3(object.model_matrix * bone_transform * vec4(in_position, 1.0));
}
vec3 processVertexNormal()
{
    return normalize(object.normal_matrix * mat3(object.bone_transform) * in_normal);
}
vec2 processVertexTexcoord()
{
    return in_texcoord;
}
