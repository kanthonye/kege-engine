vec3 getAlbedo()
{
    vec3 albedo = material.albedo.rgb;
    // Sample diffuse texture if available
    if (0 <= material.albedo_map) 
    {
        albedo = texture(MaterialTextures[ material.albedo_map ], in_texcoord).rgb;
    }
    return albedo;
}