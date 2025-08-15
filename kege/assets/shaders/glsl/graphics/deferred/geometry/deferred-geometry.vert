#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;


layout(set = 0, binding = 0) uniform ShaderCameraParams
{
    mat4 projection;
    mat4 transform;
    vec4 position;
} camera;

layout( push_constant ) uniform ObjectMatrices
{
    mat4 transform;
    mat4 rotation;
} model;

layout(location = 0) out vec3 out_positions;
layout(location = 1) out vec3 out_normals;
layout(location = 2) out vec2 out_texcoord;

void main()
{
    vec4 position = model.transform * vec4( in_position, 1.0 );
    gl_Position = camera.projection * camera.transform * position;
    gl_Position.y = -gl_Position.y;
    gl_Position.x = -gl_Position.x;
}
