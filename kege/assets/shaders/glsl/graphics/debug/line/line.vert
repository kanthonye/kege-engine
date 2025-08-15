#version 450

layout(location = 0) in vec3 _position;

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec4 position;
} camera;

void main()
{
    gl_Position = camera.projection * camera.transform * vec4( _position, 1.0 );
    gl_Position.y = -gl_Position.y;
}
