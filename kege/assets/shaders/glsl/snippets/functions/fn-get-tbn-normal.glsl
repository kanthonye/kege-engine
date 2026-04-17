// Function to decode a normal map value into a world space normal vector
vec3 getNormal()
{
    vec3 world_normal = in_normal;
    if ( 0 <= material.normal_map )
    {
        // Transform from tangent space to world space
        vec3 T = normalize(in_tangent);
        vec3 B = normalize(in_bitangent);
        vec3 N = normalize(in_normal);
        mat3 TBN = mat3(T, B, N);

        // Decode normal from [0,1] to [-1,1]
        vec3 encoded_normal = texture(MaterialTextures[ material.normal_map ], in_texcoord).rgb;
        vec3 tangent_normal = normalize( encoded_normal * 2.0 - 1.0 );

        // Transform to world space
        world_normal = TBN * tangent_normal;
    }
    return world_normal;
}
