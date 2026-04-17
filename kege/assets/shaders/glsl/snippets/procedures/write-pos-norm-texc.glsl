    // Transform the vertex position, normal, and texture coordinates by the object's model matrix and normal matrix
    out_position = object.model_matrix * vec4(in_position, 1.0);
    out_normal = normalize(object.normal_matrix * in_normal);
    out_texcoord = in_texcoord;

