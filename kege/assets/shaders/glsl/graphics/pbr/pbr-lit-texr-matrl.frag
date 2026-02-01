#version 450
#define PI 3.14159265359

layout(location = 0) in vec3 frag_position;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec2 frag_texcoord;
layout(location = 3) in vec3 camera_position;
layout(location = 4) in mat3 TBN;

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

layout(std140, set = 1, binding = 0) uniform LightBlock
{
    uint light_count;
    Light lights[MAX_LIGHTS];
}
light_data;

layout(std140, set = 2, binding = 0) uniform MaterialBlock
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
layout(set = 3, binding = 0) uniform sampler2D textures[16];

// Normal Distribution Function (NDF) for GGX/Trowbridge-Reitz.
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float cosLh2 = cosLh * cosLh;
    float denom = cosLh2 * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * denom * denom);
}

// Schlick-GGX approximation of geometric attenuation function.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float k = roughness * roughness / 2.0;
    float gaLi = cosLi / (cosLi * (1.0 - k) + k);
    float gaLo = cosLo / (cosLo * (1.0 - k) + k);
    return gaLi * gaLo;
}

// Fresnel reflectance at normal incidence using Schlick's approximation.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 pbrLighting(vec3 radiance, vec3 light_direction, vec3 normal, vec3 view_direction,
                 vec3 albedo, float roughness, float metallic, float ambient_occlusion)
{
    vec3 halfway_direction = normalize(view_direction + light_direction);

    // Calculate the cosine of the angles.
    float cosLh = max(dot(normal, halfway_direction), 0.0);
    float cosLi = max(dot(normal, light_direction), 0.0);
    float cosLo = max(dot(normal, view_direction), 0.0);

    // Compute Fresnel reflectance at normal incidence (F0).
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Calculate the Fresnel term using Schlick's approximation.
    vec3 F = fresnelSchlick(F0, max(dot(halfway_direction, view_direction), 0.0));

    // Calculate the normal distribution function (NDF) using GGX.
    float D = ndfGGX(cosLh, roughness);

    // Calculate the geometric attenuation using Schlick-GGX.
    float G = gaSchlickGGX(cosLi, cosLo, roughness);

    // Calculate the specular reflection component.
    vec3 numerator = D * G * F;
    float denominator = 4.0 * cosLi * cosLo + 0.001; // Avoid division by zero.
    vec3 specular = numerator / denominator;

    // Calculate the diffuse reflection component using Lambertian reflectance.
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic; // Only non-metallic surfaces have diffuse reflection.
    vec3 diffuse = kD * albedo / PI;

    // Apply ambient occlusion
    diffuse *= ambient_occlusion;

    // Combine the specular and diffuse components.
    return (diffuse + specular) * radiance * cosLi;
}

vec3 getNormalFromMap()
{
    if (material.normal_map == 0) {
        return normalize(frag_normal);
    }

    // Sample normal map and transform from tangent space to world space
    vec3 tangent_normal = texture(textures[material.normal_map], frag_texcoord).xyz * 2.0 - 1.0;
    return normalize(TBN * tangent_normal);
}

layout(location = 0) out vec4 out_color;

void main()
{
    // Get material properties from textures or uniforms
    vec3 albedo = material.diffuse_color;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ambient_occlusion = material.ambient_occlusion;
    float opacity = material.opacity;

    // Sample diffuse texture if available
    if (material.diffuse_map != 0) {
        albedo = texture(textures[material.diffuse_map], frag_texcoord).rgb;
    }

    // Sample metallic texture if available
    if (material.metallic_map != 0) {
        metallic = texture(textures[material.metallic_map], frag_texcoord).r;
    }

    // Sample roughness texture if available
    if (material.roughness_map != 0) {
        roughness = texture(textures[material.roughness_map], frag_texcoord).r;
    }

    // Sample ambient occlusion texture if available
    if (material.ambient_occlusion_map != 0) {
        ambient_occlusion = texture(textures[material.ambient_occlusion_map], frag_texcoord).r;
    }

    // Sample opacity texture if available
    if (material.opacity_map != 0) {
        opacity *= texture(textures[material.opacity_map], frag_texcoord).r;
    }

    // Check alpha clipping
    if (opacity < 0.5) {
        discard;
    }

    // Get normal (from normal map or vertex normal)
    vec3 normal = getNormalFromMap();
    vec3 view_direction = normalize(camera_position - frag_position);

    // Calculate PBR lighting
    vec3 lighting = vec3(0.0);

    // Add ambient lighting
    vec3 ambient = albedo * 0.03 * ambient_occlusion;
    lighting += ambient;

    // Add emissive lighting if available
    if (material.emissive_map != 0) {
        vec3 emissive = texture(textures[material.emissive_map], frag_texcoord).rgb * material.emissive_intensity;
        lighting += emissive;
    }

    // Process all lights
    for (int i = 0; i < light_data.light_count && i < MAX_LIGHTS; i++)
    {
        Light light = light_data.lights[i];
        vec3 light_direction;
        float attenuation = 1.0;

        if (light.type == LIGHT_TYPE_DIRECTIONAL) {
            light_direction = normalize(-light.direction);
        } else {
            light_direction = light.position - frag_position;
            float dist = length(light_direction);
            light_direction /= dist;

            // Calculate attenuation for point and spot lights
            attenuation = 1.0 / (1.0 + light.linear_attenuation * dist +
                                light.quadratic_attenuation * (dist * dist));

            // Spot light cone attenuation
            if (light.type == LIGHT_TYPE_SPOT) {
                float theta = dot(light_direction, normalize(-light.direction));
                if (theta > light.spot_cutoff) {
                    float epsilon = light.spot_cutoff - light.spot_cutoff * 0.9;
                    attenuation *= clamp((theta - light.spot_cutoff) / epsilon, 0.0, 1.0);
                    attenuation *= pow(theta, light.spot_exponent);
                } else {
                    attenuation = 0.0;
                }
            }

            // Range attenuation
            if (dist > light.point_range) {
                attenuation = 0.0;
            }
        }

        vec3 radiance = light.color * light.intensity * attenuation;

        lighting += pbrLighting(
            radiance,
            light_direction,
            normal,
            view_direction,
            albedo,
            roughness,
            metallic,
            ambient_occlusion
        );
    }

    // HDR tone mapping
    lighting = lighting / (lighting + vec3(1.0));

    // gamma correction
    lighting = pow(lighting, vec3(1.0 / 2.2));

    out_color = vec4(lighting, opacity);
}
