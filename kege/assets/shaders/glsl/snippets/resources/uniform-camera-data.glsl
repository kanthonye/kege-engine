// This file defines the camera data uniform buffer for the PBR shader
layout(std140, set = CAMERA_SET, binding = CAMERA_BINDING) uniform CameraData
{
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 view_projection_matrix;
    vec3 camera_position;
}
camera;

