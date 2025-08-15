#version 450

layout( push_constant ) uniform TerrainParameters
{
    mat4  transform;
    vec4  parameters[4];
} terrain;


layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec3 position;
} camera;

struct IndexBuffer
{
    ivec4 indices[ 24 ];
};
layout( set = 1, binding = 0 ) uniform IndexBuffers
{
    IndexBuffer indices[ 16 ];
};

struct Patch
{
    ivec4 position; // the position info of this patch. [xy] : poziotn, [z] : width, and [w] : index-buffer
    ivec4 layer;    // the texture image index and array layer for this patch. [x] : image index, [y] : image array layer
    vec4 color;
};
layout( std140, set = 2, binding = 0 ) buffer PatchBuffer
{
    Patch patchs[];
};


layout( set = 3, binding = 0 ) uniform sampler2DArray HeightmapImageArray[4];
layout( set = 3, binding = 1 ) uniform sampler2DArray NormalmapImageArray[4];


const vec2 vertex_position[ 9 ] = vec2[]
(
    vec2( -1.f, -1.f ),
    vec2(  0.f, -1.f ),
    vec2(  1.f, -1.f ),

    vec2( -1.f, 0.f ),
    vec2(  0.f, 0.f ),
    vec2(  1.f, 0.f ),

    vec2( -1.f, 1.f ),
    vec2(  0.f, 1.f ),
    vec2(  1.f, 1.f )
);

float getElevation( vec3 texcoord )
{
    uint image_index = patchs[ gl_InstanceIndex ].layer.x;
    if ( image_index >= 0 && 8 > image_index )
    {
        return texture( HeightmapImageArray[ image_index ], texcoord ).r;
    }
    return 0.0; // Fallback
}

vec3 getNormal( vec3 texcoord )
{
    uint image_index = patchs[ gl_InstanceIndex ].layer.x;
    if ( image_index >= 0 && 8 > image_index )
    {
        return texture( NormalmapImageArray[ image_index ], texcoord ).rbg;
    }
    return vec3(0.0); // Fallback
}

layout(location = 0) out vec4 texinfo;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec4 eye;
layout(location = 3) out vec4 color;

void main()
{
    float terrain_width     = terrain.parameters[0].x;
    float min_height        = terrain.parameters[0].y;
    float max_height        = terrain.parameters[0].z;
    vec2  terrain_center    = vec2( patchs[ gl_InstanceIndex ].layer.zw );

    vec2  patch_center      = vec2( patchs[ gl_InstanceIndex ].position.x, patchs[ gl_InstanceIndex ].position.y );
    float patch_width       = float( patchs[ gl_InstanceIndex ].position.z );
    uint  indices_id        = patchs[ gl_InstanceIndex ].position.w;
    uint  vertex_index      = indices[ indices_id ].indices[ gl_VertexIndex ].x;

    vec2  patch_vertex      = patch_width * vertex_position[ vertex_index ];


    vec2  offset;
    vec3  texcoord;

    offset                  = (patch_center + terrain_width * 0.5) - terrain_center;
    offset                  = (patch_vertex + offset) / terrain_width;
    texcoord                = vec3( offset, patchs[ gl_InstanceIndex ].layer.y );
    texinfo                 = vec4( texcoord, patchs[ gl_InstanceIndex ].layer.x );

    float height_field      = getElevation( texcoord );

    float elevation         = mix( min_height, max_height, height_field );
    vec3  position          = vec3( patch_vertex.x + patch_center.x, elevation, patch_vertex.y + patch_center.y);


    vec4 view_position      = camera.transform * vec4( position, 1.0);
    gl_Position             = camera.projection * view_position;
    gl_Position.y           = -gl_Position.y;


    vec3 colr = getNormal( texcoord );

    normal = normalize( colr * 2.0 - 1.0 );
    color = vec4(colr, height_field);
    eye.xyz = view_position.xyz;
    eye.w = length(eye);
}
