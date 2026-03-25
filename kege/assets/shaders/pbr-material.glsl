layout(std140, set = MATERIAL_SET_INDEX, binding = MATERIAL_BINDING_INDEX) uniform MaterialBlock
{
    vec3 diffuse_color;
    float opacity;

    float metallic;
    float roughness;
    float ambient_occlusion;
    float emissive_intensity;

    // Texture map presence flags (0 = no texture, 1 = has texture)
    int diffuse_map;
    int normal_map;
    int metallic_map;
    int roughness_map;
    int emissive_map;
    int ambient_occlusion_map;
    int opacity_map;
}
material;

// Texture samplers
layout(set = TEXTURE_SET_INDEX, binding = TEXTURE_BINDING_INDEX) uniform sampler2D textures[16];
