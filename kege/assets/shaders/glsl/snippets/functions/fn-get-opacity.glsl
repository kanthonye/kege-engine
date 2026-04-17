float getOpacity()
{    
    float opacity = material.albedo.a;
    // Sample opacity texture if available
    if (0 <= material.opacity_map) 
    {
        ivec2 texture_id = material.opacity_map;
        opacity = texture(MaterialTextures[ texture_id.x ], in_texcoord)[ texture_id.y ];
    }
    return opacity;
}