vec3 getEmissive()
{    
    vec3 emissive = material.emissive;
    // Sample emissive texture if available
    if (0 <= material.emissive_map) 
    {
        emissive = texture(MaterialTextures[ material.emissive_map ], in_texcoord).rgb;
    }
    return emissive;
}
