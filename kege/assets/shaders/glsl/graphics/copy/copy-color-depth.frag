#version 450
layout(location = 0) in  vec2 _texcoord;

layout (set = 0, binding = 0) uniform sampler2D src_color; // Source color texture
layout (set = 0, binding = 1) uniform sampler2D src_depth; // Source depth texture

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = texture( src_color, _texcoord );
}
