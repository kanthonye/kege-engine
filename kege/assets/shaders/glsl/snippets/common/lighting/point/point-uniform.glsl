struct PointLight
{
    vec3  color;
    float intensity;

    vec3  position;
    float point_range;

    float linear_attenuation;
    float quadratic_attenuation;
};

layout(std140, set = POINT_LIGHTS_SET, binding = POINT_LIGHTS_BINDING) uniform PointLightBuffer
{
    uint point_light_count;
    PointLight point_lights[MAX_POINT_LIGHTS];
};
