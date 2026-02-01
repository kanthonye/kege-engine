#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 view;
    vec3 position;
}
camera;

layout(push_constant) uniform ObjectMatrices
{
    mat4 transform;
    mat3 rotation;
}
model;

layout(location = 0) out vec3 frag_position;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out vec2 frag_texcoord;
layout(location = 3) out vec3 camera_position;
layout(location = 4) out mat3 TBN;

void main()
{
    camera_position = camera.position;
    frag_normal = model.rotation * normalize(normal);
    vec4 world_position = model.transform * vec4(position, 1.0);
    frag_position = world_position.xyz;
    frag_texcoord = texcoord;

    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(model.rotation * tangent);
    vec3 B = normalize(model.rotation * bitangent);
    vec3 N = frag_normal;
    TBN = mat3(T, B, N);

    gl_Position = camera.projection * camera.view * world_position;
    gl_Position.y = -gl_Position.y;
}
