#version 450

layout (local_size_x = 16, local_size_y = 16) in;

// Source color texture
layout (binding = 0) uniform sampler2D src_color;

// Source depth texture
layout (binding = 1) uniform sampler2D src_depth;

// Destination color image
layout (binding = 2, rgba8) writeonly uniform image2D dst_color;

// Destination depth image
layout (binding = 3, r32f) writeonly uniform image2D dst_depth;

void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    vec4  color = texelFetch(src_color, coord, 0.f);
    float depth = texelFetch(src_depth, coord, 0.f).r;

    imageStore(dst_color, coord, color);
    imageStore(dst_depth, coord, vec4(depth));
}

