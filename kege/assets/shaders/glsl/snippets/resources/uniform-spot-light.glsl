/**
 * This defines the uniform buffer for spot lights in the PBR shader, including
 * the properties of each spot light and an array to hold multiple spot lights
 */
struct SpotLight
{
    vec3  color;
    vec3  position
    vec3  direction;

    float linear_attenuation;
    float quadratic_attenuation;

    float spot_cutoff;
    float spot_exponent;
};
layout(std140, set = SPOT_LIGHTS_SET, binding = SPOT_LIGHTS_BINDING) uniform SpotLighBuffer
{
    uint spot_light_count;
    PointLight spot_lights[MAX_SPOT_LIGHTS];
};

