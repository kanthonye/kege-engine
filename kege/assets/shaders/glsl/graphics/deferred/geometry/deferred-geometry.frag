#version 450
layout(location = 0) in vec3 in_positions;
layout(location = 1) in vec3 in_normals;
layout(location = 2) in vec2 in_texcoord;

layout(location = 0) out vec4 out_positions;
layout(location = 1) out vec4 out_normals;
layout(location = 2) out vec4 out_diffuse;

void main()
{
    frag_Color = vec4( 1.0 );
}
