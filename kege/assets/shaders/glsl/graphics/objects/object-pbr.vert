#version 450
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec3 position;
} camera;

layout( push_constant ) uniform ObjectMatrices
{
    mat4 transform;
    mat3 rotation;
} model;

layout(location = 0) out vec3 _position;
layout(location = 1) out vec3 _normal;
layout(location = 2) out vec2 _texcoord;
layout(location = 3) out vec3 _camera_position;

void main()
{
    _camera_position = camera.position;
    _normal = mat3( model.rotation ) * in_normal;
    vec4 frag_position = model.transform * vec4( in_position, 1.0 );
    gl_Position = camera.projection * camera.transform * frag_position;
    gl_Position.y = -gl_Position.y;
    //gl_Position.x = -gl_Position.x;
    _position = frag_position.xyz;
}

