struct DirectionalLight
{
    vec3  color;
    float intensity;

    vec3 direction;
};

layout(std140, set = DIRECTIONAL_LIGHT_SET, binding = DIRECTIONAL_LIGHT_BINDING) uniform DirectionalLighting
{
    uint directional_light_count;
    DirectionalLight directional_lights[MAX_LIGHTS];
};
