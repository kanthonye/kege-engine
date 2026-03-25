
// ====== AMBIENT/IBL FUNCTION ======
// Image-based lighting (ambient) for PBR
vec3 calcAmbientIBL
(
    vec3  normal,
    vec3  view,
    vec3  albedo,
    float metallic,
    float roughness,
    float ao
)
{
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnelSchlickRoughness(max(dot(normal, view), 0.0), F0, roughness);

    // Diffuse IBL
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(IrradianceMap, normal).rgb;
    vec3 diffuse = irradiance * albedo;

    // Specular IBL
    vec3 R = reflect(-view, normal);
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefiltered_color = textureLod(PrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;

    vec2 envBRDF = texture(BRDF_LUT, vec2(max(dot(normal, view), 0.0), roughness)).rg;
    vec3 specular = prefiltered_color * (F * envBRDF.x + envBRDF.y);

    // Combine
    return (kD * diffuse + specular) * ao;
}
