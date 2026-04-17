    // Integrate the contribution of all spot lights in the scene using the PBR lighting model
    lighting = vec3(0.0);
    for (int i = 0; i < spot_light_count && i < MAX_SPOT_LIGHTS; i++)
    {
        Light light = spot_lights[i];

        vec3 direction = light.position - position;
        float dist = length(direction);
        direction /= dist;

        // Calculate attenuation for point and spot lights
        float attenuation = 1.0 / (1.0 + light.linear_attenuation * dist + light.quadratic_attenuation * (dist * dist));

        float theta = dot(direction, normalize(-light.direction));
        if (theta > light.spot_cutoff)
        {
            float epsilon = light.spot_cutoff - light.spot_cutoff * 0.9;
            attenuation *= clamp((theta - light.spot_cutoff) / epsilon, 0.0, 1.0);
            attenuation *= pow(theta, light.spot_exponent);
        }
        else
        {
            attenuation = 0.0;
        }

        vec3 radiance = light.color * light.intensity * attenuation;

        lighting += pbrLighting
        (
            normal,
            view,
            direction,
            light.color,
            albedo,
            metallic,
            roughness,
            ao
        );
    }
    final_color += lighting;
