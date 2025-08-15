#version 450

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec4 position;
} camera;

layout(set = 1, binding = 0) buffer PointBuffer
{
    mat2x4 data[];
};

const vec2 offsets[ 4 ] = vec2[]
(
    vec2( 1.f,  1.f),
    vec2(-1.f,  1.f),
    vec2( 1.f, -1.f),
    vec2(-1.f, -1.f)
);

layout(location = 0) out vec2 _offsets;
layout(location = 1) out vec4 _color;

void main()
{
    _offsets = offsets[ gl_VertexIndex ];
    _color = data[ gl_InstanceIndex ][1];
    vec4 translation = data[ gl_InstanceIndex ][0];

    mat3 billboard_matrix = transpose( mat3( camera.transform ) );
    vec3 vertex_position_3d = translation.xyz + billboard_matrix * vec3( _offsets * translation.w, 0.f );
    gl_Position = camera.projection * camera.transform * vec4( vertex_position_3d, 1.0 );
    gl_Position.y = -gl_Position.y;
}
