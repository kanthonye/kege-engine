layout(std140, set = OBJECT_SET, binding = OBJECT_BINDING) uniform ObjectData
{
    mat4 model_matrix;
    mat4 normal_matrix;
}
object;
