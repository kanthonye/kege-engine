    // Integrate the contribution of all directional lights in the scene using the PBR lighting model
    lighting = vec3(0.0);
    for (int i = 0; i < directional_light_count && i < MAX_DIRECTIONAL_LIGHTS; i++)
    {
        lighting += pbrLighting
        (
            normal,
            view,
            light.direction,
            light.color,
            albedo,
            metallic,
            roughness,
            ao
        );
    }
    final_color += lighting;
