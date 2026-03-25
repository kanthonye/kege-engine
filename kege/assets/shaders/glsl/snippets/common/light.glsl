#define MAX_LIGHTS 10
const int LIGHT_TYPE_SPOT = 0;
const int LIGHT_TYPE_POINT = 1;
const int LIGHT_TYPE_DIRECTIONAL = 2;

struct Light
{
    vec3 color;
    float intensity;

    vec3 position;
    float point_range;

    vec3 direction;
    int type;

    float spot_cutoff;
    float spot_exponent;

    float linear_attenuation;
    float quadratic_attenuation;
};

layout(std140, set = LIGHTS_SET_INDEX, binding = LIGHTS_BINDING_INDEX) uniform LightBlock
{
    uint light_count;
    Light lights[MAX_LIGHTS];
}
light_data;
