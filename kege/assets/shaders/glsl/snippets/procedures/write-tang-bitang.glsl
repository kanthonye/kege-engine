    // Apply the object's normal matrix to the bitangent and tangent vectors
    out_bitangent = normalize(object.normal_matrix * in_bitangent);
    out_tangent = normalize(object.normal_matrix * in_tangent);
