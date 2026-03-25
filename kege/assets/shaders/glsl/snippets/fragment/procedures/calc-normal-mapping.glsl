
    mat3 TBN = mat3(in_tangent, in_bitangent, in_normal);
    vec3 mapped_normal = texture(NormalTexture, in_texcoord).xyz * 2.0 - 1.0;
    vec3 normal = normalize(TBN * mapped_normal);
