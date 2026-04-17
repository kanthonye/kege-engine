    vec3 view = in_position;
    vec3 albedo = vec3(1.0f);
    float metallic = 0.1f;
    float roughness = 0.5f;
    vec3 view_direction = normalize(camera_position - in_position);

    // Calculate PBR lighting
    vec3 lighting = vec3(0.0);

    // Add ambient lighting
    lighting += albedo * 0.03;
