// ====== MAIN PBR FUNCTION ======
// Calculates PBR lighting for ONE light
vec3 pbrLighting // calculateSourceLighting, calculateAmbientLighting
(
    vec3 N,              // World normal
    vec3 V,              // View direction (world space)
    vec3 L,              // Light direction (world space)
    vec3 light_color,    // Light color/intensity
    vec3 albedo,         // Surface albedo (diffuse color)
    float metallic,      // 0.0 = dielectric, 1.0 = metal
    float roughness,     // 0.0 = smooth, 1.0 = rough
    float ao             // Ambient occlusion
)
{
    // Half vector
    vec3 H = normalize(V + L);

    // Basic dot products
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    if (NdotL <= 0.0)
    {
        return vec3(0.0);  // Light behind surface
    }

    // Calculate reflectance at normal incidence (F0)
    // For dielectrics: ~0.04, for metals: albedo
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Cook-Torrance BRDF components
    float NDF = distributionGGX(N, H, roughness);
    float G   = geometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    // BRDF numerator
    vec3 numerator = NDF * G * F;

    // BRDF denominator
    float denominator = 4.0 * max(NdotV, 0.0) * NdotL;
    vec3 specular = numerator / max(denominator, 0.0001);

    // Diffuse (kD) and specular (kS) factors
    vec3 kS = F;  // Specular contribution
    vec3 kD = vec3(1.0) - kS;

    // Metals have no diffuse reflection
    kD *= 1.0 - metallic;

    // Final lighting
    return (kD * albedo / PI + specular) * light_color * NdotL;
}
