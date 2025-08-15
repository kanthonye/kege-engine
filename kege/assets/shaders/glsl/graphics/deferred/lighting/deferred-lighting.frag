#version 450

layout(set = 0, binding = 0) uniform sampler2D PositionBuffer;
layout(set = 0, binding = 1) uniform sampler2D NormalBuffer;
layout(set = 0, binding = 2) uniform sampler2D AlbedoBuffer;
layout(set = 0, binding = 3) uniform sampler2D DepthBuffer;

layout(location = 0) out vec4  out_color;
layout(location = 0) in  vec2  in_texcoord;

void main()
{
    vec4 position = texture(PositionBuffer, in_texcoord);
    vec4 normal   = texture(NormalBuffer, in_texcoord);
    vec4 albedo   = texture(AlbedoBuffer, in_texcoord);
    gl_FragDepth  = texture(DepthBuffer, in_texcoord).r;

    vec3 l = normalize( vec3(30, 100, 60) );
    float diffuse = max(0.1, dot( normal.xyz, l ));
    out_color = vec4( diffuse * vec3(1), 1 );
}
