float getMetallic()
{
    float metallic = material.metallic;
    // Sample metallic texture if available
    if (material.metallic_map != 0) 
    {
        ivec2 texture_id = material.metallic_map;
        metallic = texture(MaterialTextures[ texture_id.x ], in_texcoord)[ texture_id.y ];
    }
    return metallic;
}