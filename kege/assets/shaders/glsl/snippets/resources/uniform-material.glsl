// This defines the uniform buffer for material properties in the PBR shader
layout(std140, set = MATERIAL_SET, binding = 0) uniform MaterialBuffer
{
    // Base parameters
    vec4 albedo;           // Albedo/diffuse color
    vec3 emissive;

    // PBR Metallic-Roughness parameters
    float metallic;
    float roughness;
    float ambient_occlusion;
    float ior; // Index of refraction

    // Other parameters
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

    int albedo_map;
    int emissive_map;
    int normal_map;
    int subsurface_map;
    ivec2 metallic_map;
    ivec2 roughness_map;
    ivec2 opacity_map;
    ivec2 ambient_occlusion_map;
    ivec2 sheen_map;
    ivec2 anisotropy_map;
    ivec2 clear_coat_map;
}
material;

// Texture samplers for the material properties
layout(set = MATERIAL_SET, binding = 1) uniform sampler2D MaterialTextures[8];

