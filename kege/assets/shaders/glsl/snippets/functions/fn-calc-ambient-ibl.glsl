/** 
 * ====== AMBIENT/IBL FUNCTION ======
 * This function calculates the ambient lighting contribution using Image-Based Lighting (IBL) 
 * for a given surface point. It combines both diffuse and specular IBL components based on the 
 * material properties and the view direction. 
 */
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
    // Calculate the Fresnel reflectance at normal incidence (F0) and the Fresnel term (F)
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    // Use the roughness-aware Fresnel-Schlick approximation for better accuracy with rough materials
    vec3 F = fresnelSchlickRoughness(max(dot(normal, view), 0.0), F0, roughness);

    // Diffuse IBL
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    // Sample the irradiance map for diffuse IBL
    vec3 irradiance = texture(IrradianceMap, normal).rgb;
    // Calculate the diffuse contribution from IBL
    vec3 diffuse = irradiance * albedo;

    // Specular IBL
    vec3 R = reflect(-view, normal);
    const float MAX_REFLECTION_LOD = 4.0;

    // Sample the pre-filtered environment map for specular IBL using the reflection vector and roughness
    vec3 prefiltered_color = textureLod(PrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;

    // Sample the BRDF LUT to get the Fresnel and geometry terms for the specular IBL contribution
    vec2 envBRDF = texture(BRDF_LUT, vec2(max(dot(normal, view), 0.0), roughness)).rg;

    // Calculate the specular contribution from IBL using the pre-filtered color, Fresnel term, and BRDF LUT
    vec3 specular = prefiltered_color * (F * envBRDF.x + envBRDF.y);

    // Combine the diffuse and specular IBL contributions, modulated by the ambient occlusion (ao) factor
    return (kD * diffuse + specular) * ao;
}
