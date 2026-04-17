    // Transform the vertex position, normal, and texture coordinates by the bone transformation and normal matrix
    out_position = object.model_matrix * bone_transform * vec4(in_position, 1.0);
    out_normal = normalize(bone_normal_matrix * in_normal);
    out_texcoord = in_texcoord;

