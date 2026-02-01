#version 450

// Layout matches your JSON vertex_layouts
layout(location = 0) in vec3 in_position;

// Layout matches your JSON uniforms (Set 0, Binding 0)
layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 view;
}
camera;

// Layout matches your JSON push_constants
layout(push_constant) uniform ObjectMatrices
{
    mat4 model;
}
object;

void main()
{
    // Standard MVP transformation
    gl_Position = camera.projection * camera.view * object.model * vec4(in_position, 1.0);
}
