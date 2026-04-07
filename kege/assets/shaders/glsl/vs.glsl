#version 450
#extension GL_ARB_separate_shader_objects : enable

#define FEATURE_NORMAL_MAPPING
#define CAMERA_SET 0
#define CAMERA_BINDING 0
#define OBJECT_SET 1
#define OBJECT_BINDING 0
#define MATERIAL_SET 2

#define INPUT_POSITION  0
#define INPUT_TEXCOORD 1
#define INPUT_NORMAL 2
#define INPUT_TANGENT 3
#define INPUT_BITANGENT 4

#define OUTPUT_POSITION  0
#define OUTPUT_TEXCOORD 1
#define OUTPUT_NORMAL 2
#define OUTPUT_TANGENT  3
#define OUTPUT_BITANGENT 4

layout(location = INPUT_POSITION) in vec3 in_position;
layout(location = INPUT_TEXCOORD) in vec2 in_texcoord;
layout(location = INPUT_NORMAL) in vec3 in_normal; 
layout(location = INPUT_TANGENT) in vec3 in_tangent;
layout(location = INPUT_BITANGENT) in vec3 in_bitangent; 

layout(location = OUTPUT_POSITION )  out vec3 out_position; 
layout(location = OUTPUT_TEXCOORD )  out vec2 out_texcoord; 
layout(location = OUTPUT_NORMAL ) out vec3 out_normal; 
layout(location = OUTPUT_TANGENT )   out vec3 out_tangent; 
layout(location = OUTPUT_BITANGENT ) out vec3 in_bitangent; 

// This file defines the camera data uniform buffer for the PBR shader
layout(std140, set = CAMERA_SET, binding = CAMERA_BINDING) uniform CameraData
{
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 view_projection_matrix;
    vec3 camera_position;
}
camera; 

// This defines the uniform buffer for object data in the PBR shader, including the model matrix and normal matrix
layout(std140, set = OBJECT_SET, binding = OBJECT_BINDING) uniform ObjectData
{
    mat4 model_matrix;
    mat4 normal_matrix;
}
object; 


void main() {
    // Process vertex based on renderer type
    // Transform the vertex position, normal, and texture coordinates by the object's model matrix and normal matrix
    out_position = object.model_matrix * vec4(in_position, 1.0);
    out_normal = normalize(object.normal_matrix * in_normal);
    out_texcoord = in_texcoord;
 
    // Apply the object's normal matrix to the bitangent and tangent vectors
    out_bitangent = normalize(object.normal_matrix * in_bitangent);
    out_tangent = normalize(object.normal_matrix * in_tangent); 
    // Final vertex output, this is the last step of the vertex shader, it will output the final position of the vertex in clip space.
    gl_Position = camera.projection * camera.view * vec4(out_position, 1.0); 
}
