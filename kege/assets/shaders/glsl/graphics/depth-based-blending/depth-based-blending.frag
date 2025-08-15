#version 450

layout(set = 0, binding = 0) uniform sampler2D ColorAttachment;
layout(set = 0, binding = 1) uniform sampler2D DepthAttachment;

layout(location = 0) out vec4  _final_color;
layout(location = 0) in  vec2  _texcoords;

void main()
{
    // Output color and depth values to the frame buffer
    _final_color = texture( ColorAttachment, _texcoords );
    gl_FragDepth = texture( DepthAttachment, _texcoords ).r;
}

