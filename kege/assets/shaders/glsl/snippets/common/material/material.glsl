// ====== MATERIAL DATA STRUCTURE ======
// This is the ACTUAL material data
layout(std140, set = MATERIAL_SET, binding = 0) uniform MaterialBuffer
{
    // Base parameters
    vec4 albedo;           // Albedo/diffuse color

    // PBR Metallic-Roughness parameters
    float metallic;
    float roughness;
    float ambient_occlusion;
    float ior; // Index of refraction

    // PBR Specular-Glossiness parameters
    vec4 specular; // RGB + intensity

    // Other parameters
    vec3  emissive;
    float alpha_cutoff;

    // Subsurface scattering
    vec3  subsurface;
    float subsurface_thickness;

    // Sheen (cloth)
    vec3  sheen;
    float sheen_roughness;

    // Anisotropy
    vec2  anisotropy_direction;
    float anisotropy;

    // UV transforms
    vec4 uv_scale_offset; // xy = scale, zw = offset

    // Transmission
    float transmission;

    // Clear coat (optional)
    float clear_coat;
    float clear_coat_roughness;

};

// ====== TEXTURE BINDINGS ======
// Textures are separate from the BRDF!
layout(set = MATERIAL_SET, binding = 1) uniform sampler2D BaseColorTexture;
layout(set = MATERIAL_SET, binding = 3) uniform sampler2D MetallicRoughnessTexture;
layout(set = MATERIAL_SET, binding = 4) uniform sampler2D OcclusionTexture;
layout(set = MATERIAL_SET, binding = 5) uniform sampler2D EmissiveTexture;

// Optional textures
#ifdef FEATURE_CLEAR_COAT
layout(set = MATERIAL_SET, binding = 5) uniform sampler2D ClearCoatTexture;
layout(set = MATERIAL_SET, binding = 6) uniform sampler2D ClearCoatRoughnessTexture;
#endif

#ifdef FEATURE_ANISOTROPY
layout(set = MATERIAL_SET, binding = 7) uniform sampler2D AnisotropyTexture;
#endif
