    // Integrate the contribution of all point lights in the scene using the PBR lighting model
    lighting = vec3(0.0);
    for (int i = 0; i < point_light_count && i < MAX_POINT_LIGHTS; i++)
    {
        Light light = point_lights[i];

        vec3 light_direction = light.position - position;
        float dist = length(direction);
        light_direction /= dist;

        // Calculate attenuation for point and spot lights
        float attenuation = 1.0 / (1.0 + light.linear_attenuation * dist + light.quadratic_attenuation * (dist * dist));

        vec3 radiance = light.color * light.intensity * attenuation;

        lighting += pbrLighting
        (
            normal,
            view,
            light_direction,
            light.color,
            albedo,
            metallic,
            roughness,
            ao
        );
    }
    final_color += lighting;
