#version 450

layout (location = 0) in vec3  in_Position;
layout (location = 1) in vec4  in_Color;
layout (location = 4) in vec4  in_Sprite;
layout (location = 2) in float in_Size;
layout (location = 3) in float in_Rotation;

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec3 position;
} camera;

layout(location = 0) out vec4 _color;

const vec2 vertex_position[ 4 ] = vec2[]
(
    vec2( 1.f,  1.f),
    vec2(-1.f,  1.f),
    vec2( 1.f, -1.f),
    vec2(-1.f, -1.f)
);

void main()
{
    _color = in_Color;

    // Extract the right and up vectors from the view matrix
    vec3 camera_right = vec3(camera.transform[0][0], camera.transform[1][0], camera.transform[2][0]);
    vec3 camera_up    = vec3(camera.transform[0][1], camera.transform[1][1], camera.transform[2][1]);

    // Calculate the offset from the particle position in world space
    vec2 quad_point = vertex_position[ gl_VertexIndex ] * in_Size * 0.5;

    // Calculate the world position of the billboard vertex
    vec3 billboard_world_position = in_Position.xyz + camera_right * quad_point.x + camera_up * quad_point.y;


//    vec3 position = in_Position.xyz + quad_point * 0.5;

    gl_Position = camera.projection * camera.transform * vec4( billboard_world_position, 1.0 );
    gl_Position.y = -gl_Position.y;

//    vec4 world_pos_cam_space = camera.transform * vec4(in_Position.xyz, 1.0);
//    vec3 billboard_offset = billboard_matrix * vec3(quad_point, 0.0);
//    gl_Position = camera.projection * (world_pos_cam_space + vec4(billboard_offset, 1.0));
//    gl_Position.y = -gl_Position.y;
}
