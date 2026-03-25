    for (int i = 0; i < directional_light_count && i < MAX_LIGHTS; i++)
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
