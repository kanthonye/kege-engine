#version 450

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec3 position;
} camera;

layout( push_constant ) uniform ObjectMatrices
{
    mat4 transform;
    mat4 rotation;
} model;

layout(location = 0) out vec3 normal;

void main()
{
    gl_Position = camera.projection * camera.transform * model.transform * vec4( _position, 1.0 );
    gl_Position.y = -gl_Position.y;

    normal = _normal;
}

