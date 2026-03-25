    vec3  normal = in_normal;
    vec3  view = in_position;
    vec3  albedo = vec3(1.f, 0.0f, 1.f);
    float metallic = 0.4f; // 0.0 = dielectric, 1.0 = metal
    float roughness = 0.01f; // 0.0 = smooth, 1.0 = rough
    float ao = 1.f; // Ambient occlusion
