#version 450

/**
 * tells the driver how to distribute the workload on the GPU. In our case, we are processing tiles of 16x16 pixels
 */
layout (local_size_x = 16, local_size_y = 16) in;

layout(set = 0, binding = 0, rgba8) uniform readonly  image2D Heightmap;
layout(set = 0, binding = 1, rgba8) uniform writeonly image2D Normalmap;

layout( push_constant ) uniform constants
{
    float strength;
} params;

void main()
{
    float h[9];

    ivec2 dim = imageSize( Heightmap );
    ivec2 uv = ivec2( gl_GlobalInvocationID.xy );

    //  0----1----2
    //  |    |    |
    //  3----4----5
    //  |    |    |
    //  6----7----8

    vec4 color = imageLoad( Heightmap, uv );

    h[0] = imageLoad( Heightmap, uv + ivec2(-1,-1) ).a;
    h[1] = imageLoad( Heightmap, uv + ivec2( 0,-1) ).a;
    h[2] = imageLoad( Heightmap, uv + ivec2( 1,-1) ).a;
    h[3] = imageLoad( Heightmap, uv + ivec2(-1, 0) ).a;
    h[4] = color.a;
    h[5] = imageLoad( Heightmap, uv + ivec2( 1, 0) ).a;
    h[6] = imageLoad( Heightmap, uv + ivec2(-1, 1) ).a;
    h[7] = imageLoad( Heightmap, uv + ivec2( 0, 1) ).a;
    h[8] = imageLoad( Heightmap, uv + ivec2( 1, 1) ).a;
 
    vec3 normal = vec3(0.0);
    normal.z = 1.0 / params.strength;
    normal.x = h[0] + 2.0*h[3] + h[5] - h[2] - 2.0*h[4] - h[7];
    normal.y = h[0] + 2.0*h[1] + h[2] - h[5] - 2.0*h[6] - h[7];
    normal = normalize( normal ) * 0.5 + 0.5;
 
    color.x = normal.x;
    color.y = normal.y;
    color.z = normal.z;

    imageStore( Normalmap, ivec2( gl_GlobalInvocationID.xy ), color );
}
