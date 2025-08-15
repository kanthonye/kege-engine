#version 450
layout( set = 3, binding = 0 ) uniform sampler2DArray HeightmapImageArray[8];
layout(location = 0) in  vec4 texcoord;
layout(location = 1) in  vec3 normal;
layout(location = 2) in  vec4 eye;
layout(location = 3) in  vec4 color;

vec3 applyFog
(
    in vec3  col,   // color of pixel
    in float t,     // distance to point
    in vec3  rd,    // camera to point
    in vec3  lig    // sun direction
)
{
    float fog_density = 0.000125;
    float fog_amount = 1.0 - exp(-t * fog_density);
    float sun_amount = max( dot(rd, lig), 0.0 );
    vec3  fog_color  = mix( vec3(0.5,0.6,0.7) /* blue */, vec3(1.0,0.9,0.7) /* yellow */, pow(sun_amount,8.0) );
    return mix( col, fog_color, fog_amount );
}

const vec3 dirt = vec3(0.471, 0.384, 0.314);
const vec3 rock = vec3(0.302, 0.333, 0.349);
const vec3 grass = vec3(0, 0.522, 0.027);
const vec3 up = vec3(0, 1, 0);

layout(location = 0) out vec4 frag_Color;

void main()
{
    vec3 norm = normalize(normal);

    float edge0 = 0.6;
    float edge1 = 1.5;
    float steepness = 1.0 - dot(norm, up);
    steepness = smoothstep(edge0, edge1, steepness);
    vec3 biome_color = mix( vec3(0.329, 0.525, 0.016), vec3(0.212, 0.231, 0.216), steepness );



    // Normalize the normal and light direction
    vec3 light_direction = normalize(-vec3(80, 10, 50)); // Direction from the surface to the light source
    // Calculate the diffuse component
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = diff * biome_color;

    diffuse = applyFog
    (
     diffuse,   // color of pixel
     eye.w,     // distance to point
     normalize(eye.xyz),    // camera to point
     light_direction    // sun direction
    );

    frag_Color = vec4(diffuse,1);
//    frag_Color = vec4(1);
//    frag_Color = color;vec4(texcoord, 0, 1);
//    frag_Color = vec4(vec3(texture( HeightmapImageArray[ int(texcoord.w) ], texcoord.xyz ).r),1);
}

