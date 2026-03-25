
    // Get material properties from textures or uniforms
    vec3 albedo = material.diffuse_color;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ambient_occlusion = material.ambient_occlusion;
    float opacity = material.opacity;

    // Sample diffuse texture if available
    if (material.diffuse_map != 0) {
        albedo = texture(textures[material.diffuse_map], frag_texcoord).rgb;
    }

    // Sample metallic texture if available
    if (material.metallic_map != 0) {
        metallic = texture(textures[material.metallic_map], frag_texcoord).r;
    }

    // Sample roughness texture if available
    if (material.roughness_map != 0) {
        roughness = texture(textures[material.roughness_map], frag_texcoord).r;
    }

    // Sample ambient occlusion texture if available
    if (material.ambient_occlusion_map != 0) {
        ambient_occlusion = texture(textures[material.ambient_occlusion_map], frag_texcoord).r;
    }

    // Sample opacity texture if available
    if (material.opacity_map != 0) {
        opacity *= texture(textures[material.opacity_map], frag_texcoord).r;
    }

    // Check alpha clipping
    if (opacity < 0.5) {
        discard;
    }

    vec3 view_direction = normalize(camera_position - frag_position);

    // Calculate PBR lighting
    vec3 lighting = vec3(0.0);

    // Add ambient lighting
    vec3 ambient = albedo * 0.03 * ambient_occlusion;
    lighting += ambient;
