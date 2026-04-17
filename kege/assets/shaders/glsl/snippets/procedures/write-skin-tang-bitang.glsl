    // Apply the bone normal matrix to the bitangent and tangent vectors
    out_bitangent = normalize(bone_normal_matrix * in_bitangent);
    out_tangent = normalize(bone_normal_matrix * in_tangent);
