#version 450

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec4 position;
} camera;

layout( push_constant ) uniform ObjectBlock
{
    mat4 transform;
    mat3 rotation;
} model;

const vec2 vertex_position[ 4 ] = vec2[]
(
 vec2( 1.f,  1.f),
 vec2(-1.f,  1.f),
 vec2( 1.f, -1.f),
 vec2(-1.f, -1.f)
);

layout(location = 0) out vec2 _vertex_position;

void main()
{
    _vertex_position = vertex_position[ gl_VertexIndex ];
    mat3 billboard_matrix = transpose(mat3(camera.transform));
    vec3 vertex_position_3d = billboard_matrix * vec3( _vertex_position, 0.f );
    vec4 vertex_position_4d = model.transform * vec4( vertex_position_3d * 0.4, 1.0 );
    gl_Position = camera.projection * camera.transform * vertex_position_4d;
    gl_Position.y = -gl_Position.y;
}
