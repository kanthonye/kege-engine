vec3 getNormalFromMap()
{
    if (material.normal_map == 0) {
        return normalize(frag_normal);
    }

    // Sample normal map and transform from tangent space to world space
    vec3 tangent_normal = texture(textures[material.normal_map], frag_texcoord).xyz * 2.0 - 1.0;
    return normalize(TBN * tangent_normal);
}

void main()
{
#ifdef INCLUDE_PBR_MATERIAL
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
#endif

#ifdef INCLUDE_PBR_LIGHTING
    // Get normal (from normal map or vertex normal)
    vec3 normal = getNormalFromMap();
    vec3 view_direction = normalize(camera_position - frag_position);

    // Calculate PBR lighting
    vec3 lighting = vec3(0.0);

    // Add ambient lighting
    vec3 ambient = albedo * 0.03 * ambient_occlusion;
    lighting += ambient;

    // Add emissive lighting if available
    if (material.emissive_map != 0)
    {
        vec3 emissive = texture(textures[material.emissive_map], frag_texcoord).rgb * material.emissive_intensity;
        lighting += emissive;
    }

    // Process all lights
    for (int i = 0; i < light_data.light_count && i < MAX_LIGHTS; i++)
    {
        Light light = light_data.lights[i];
        vec3 light_direction;
        float attenuation = 1.0;

        if (light.type == LIGHT_TYPE_DIRECTIONAL)
        {
            light_direction = normalize(-light.direction);
        }
        else
        {
            light_direction = light.position - frag_position;
            float dist = length(light_direction);
            light_direction /= dist;

            // Calculate attenuation for point and spot lights
            attenuation = 1.0 / (1.0 + light.linear_attenuation * dist +
                                light.quadratic_attenuation * (dist * dist));

            // Spot light cone attenuation
            if (light.type == LIGHT_TYPE_SPOT)
            {
                float theta = dot(light_direction, normalize(-light.direction));
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
            }

            // Range attenuation
            if (dist > light.point_range)
            {
                attenuation = 0.0;
            }
        }

        vec3 radiance = light.color * light.intensity * attenuation;

        lighting += pbrLighting
        (
            radiance,
            light_direction,
            normal,
            view_direction,
            albedo,
            roughness,
            metallic,
            ambient_occlusion
        );
    }

    // HDR tone mapping
    lighting = lighting / (lighting + vec3(1.0));

    // gamma correction
    lighting = pow(lighting, vec3(1.0 / 2.2));

    out_color = vec4(lighting, opacity);
#endif
}
