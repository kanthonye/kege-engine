    // Calculate the bone transformation matrix by blending the transformations of the influencing bones based on their weights
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
    
    // Calculate the bone normal matrix by combining the object's normal matrix with the bone transformation
    mat3 bone_normal_matrix = object.normal_matrix * mat3(object.bone_transform);

    // Transform the vertex position, normal, and texture coordinates by the bone transformation and normal matrix
    out_position = object.model_matrix * bone_transform * vec4(in_position, 1.0);
    out_normal = normalize(bone_normal_matrix * in_normal);
    out_texcoord = in_texcoord;

