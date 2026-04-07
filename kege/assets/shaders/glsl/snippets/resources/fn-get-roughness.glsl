float getRoughness()
{
    float roughness = material.roughness;
    // Sample roughness texture if available
    if (material.roughness_map != 0) 
    {
        ivec2 texture_id = material.roughness_map;
        roughness = texture(MaterialTextures[ texture_id.x ], in_texcoord)[ texture_id.y ];
    }
    return roughness;
}
