#version 450

layout (local_size_x = 16, local_size_y = 16) in;

// Source depth texture
layout (binding = 0) uniform sampler2D src_depth;

// Destination depth image
layout (binding = 1, r32f) writeonly uniform image2D dst_depth;

void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    float depth = texelFetch(src_depth, coord, 0.f).r;
    imageStore(dst_depth, coord, vec4(depth));
}
