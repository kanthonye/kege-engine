    vec3 normal = in_normal;
    if (material.use_normal_map)
    {
        vec3 encoded_normal = texture(NormalTexture, in_texcoord).rgb;
        normal = sampleNormalMap(encoded_normal, in_tangent, in_bitangent, in_normal);
    }

    // ===== ENCODE NORMALS (compress from [-1,1] to [0,1]) =====
    vec3 normal = world_normal * 0.5 + 0.5;

    // Calculate the normal in tangent space using the TBN matrix and the normal map
    mat3 TBN = mat3(, , );
    vec3 mapped_normal = texture(NormalTexture, in_texcoord).xyz * 2.0 - 1.0;
    vec3 normal = normalize(TBN * mapped_normal);
