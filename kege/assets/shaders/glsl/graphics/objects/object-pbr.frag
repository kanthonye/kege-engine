#version 450
#define PI 3.14159265359

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec2 _texcoord;
layout(location = 3) in vec3 _camera_position;

#define MAX_LIGHTS 10;
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
    uint  light_count;
    Light lights[ 100 ];
};



layout(std140, set = 2, binding = 0) uniform MaterialBlock
{
    vec3  diffuse_color;         // Base color of the material
    float opacity;               // Transparency property

    float metallic;              // Metallic property
    float roughness;             // Roughness property
    float ambient_occlusion;     // Ambient occlusion factor
    float emissive_intensity;    // Emissive intensity

    // Texture maps
    
    int diffuse_map;             // Index to the base color texture map
    int normal_map;              // Index to the normal texture map
    int metallic_map;            // Index to the metallic texture map
    int roughness_map;           // Index to the roughness texture map
    
    int emissive_map;            // Index to the emissive texture map
    int ambient_occlusion_map;   // Index to the ambient occlusion texture map
    int opacity_map;
}
material;



// Normal Distribution Function (NDF) for GGX/Trowbridge-Reitz.
float ndfGGX(float cosLh, float roughness)
{
    float  alpha = roughness * roughness;
    float  alpha2 = alpha * alpha;
    float  cosLh2 = cosLh * cosLh;
    float  denom = cosLh2 * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * denom * denom);
}

// Schlick-GGX approximation of geometric attenuation function.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float  k = roughness * roughness / 2.0;
    float  gaLi = cosLi / (cosLi * (1.0 - k) + k);
    float  gaLo = cosLo / (cosLo * (1.0 - k) + k);
    return gaLi * gaLo;
}

// Fresnel reflectance at normal incidence using Schlick's approximation.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 pbrLighting( vec3 radiance, vec3 light_direction, vec3 normal, vec3 view_direction, vec3 albedo, float roughness, float metallic )
{
    vec3 halfway_direction = normalize( view_direction + light_direction );

    // Calculate the cosine of the angles.
    float cosLh = max(dot(normal, halfway_direction), 0.0);
    float cosLi = max(dot(normal, light_direction), 0.0);
    float cosLo = max(dot(normal, view_direction), 0.0);

    // Compute Fresnel reflectance at normal incidence (F0).
    vec3  F0 = mix(vec3(0.04), albedo, metallic);

    // Calculate the Fresnel term using Schlick's approximation.
    vec3  F = fresnelSchlick(F0, max(dot(halfway_direction, view_direction), 0.0));

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

    // Combine the specular and diffuse components.
    return (diffuse + specular) * radiance * cosLi;
}

layout(location = 0) out vec4 out_color;

void main()
{
    // Sample usage of pbrLighting in the main shader function.

    // Assume the following variables are defined elsewhere in your shader code.
    vec3  normal = normalize( _normal ); // World space normal of the fragment.
    vec3  view_direction = normalize( _camera_position - _position ); // World space view direction (camera position - fragment position).
    vec3  albedo = material.diffuse_color;/// vec3( 1, 0, 0.7 ); // Base color of the material.
    float roughness = 0.0;//material.roughness; // Roughness of the material.
    float metallic = 0.0;//material.metallic; // Metallicity of the material.

    // Calculate the PBR lighting.
    vec3  lighting = vec3( 0.0 );
    vec3  light_direction;
    float attenuation;
    vec3  radiance;

    for ( int i = 0; i < light_count; i++ )
    {
        light_direction = lights[ i ].position - _position;
        float dist = length( light_direction );
        light_direction /= dist;

        attenuation  = 1.0;
        attenuation += lights[ i ].linear_attenuation * dist;
        attenuation += lights[ i ].quadratic_attenuation * (dist * dist);
        attenuation  = 1.0 / attenuation;

        radiance = lights[ i ].color * attenuation;

        lighting += pbrLighting
        (
            radiance,
            light_direction,
            normal,
            view_direction,
            albedo,
            roughness,
            metallic
        );
    }

    // HDR tone mapping
    //lighting = lighting / (lighting + vec3(1.0));

    // gamma correction
    //lighting = pow(lighting, vec3(1.0/2.2));

    out_color = vec4( lighting, 1.0 );
    //out_color = vec4( 1 );
}
