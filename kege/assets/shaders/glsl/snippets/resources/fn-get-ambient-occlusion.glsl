float getAmbientOcclusion()
{    
    float ambient_occlusion = material.ambient_occlusion;
    // Sample ambient occlusion texture if available
    if (0 <= material.ambient_occlusion_map) 
    {
        ivec2 texture_id = material.ambient_occlusion_map;
        ambient_occlusion = texture(MaterialTextures[ texture_id.x ], in_texcoord)[ texture_id.y ];
    }
    return ambient_occlusion;
}