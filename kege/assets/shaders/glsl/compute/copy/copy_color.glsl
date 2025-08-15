#version 450

layout (local_size_x = 16, local_size_y = 16) in;

// Source texture
layout (binding = 0) uniform sampler2D src_image;

// Destination image
layout (binding = 1, rgba8) writeonly uniform image2D dst_image;

void main()
{
    ivec2 texel_coord = ivec2( gl_GlobalInvocationID.xy );

    // Read from source texture
    vec4 color = texelFetch( src_image, texel_coord, 0 );

    // Write to destination image
    imageStore( dst_image, texel_coord, color );
}
