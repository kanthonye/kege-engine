#version 450

//layout(location = 0) out vec4 _color;
layout(location = 0) in  vec4 _vertex_position;

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec3 position;
} camera;


layout( push_constant ) uniform ObjectMatrices
{
    mat4  transform;
    mat4  rotation;
}
model;


struct PatchData
{
    vec4 translation_scale;
};
layout( std140, set = 1, binding = 0 ) buffer PatchDataBuffer
{
    PatchData patch_data[];
};


void main()
{
    //gl_Position = camera.projection * camera.transform * model.transform * vec4( _vertex_position.xyz, 1.0);

    float radius  = model.rotation[3][0];
    vec3 translation  = patch_data[ gl_InstanceIndex ].translation_scale.xyz;
    float scale = patch_data[ gl_InstanceIndex ].translation_scale.w;
    vec3 position = translation + _vertex_position.xyz * scale;

    float xx = position.x * position.x;
    float yy = position.y * position.y;
    float zz = position.z * position.z;
    position.x = position.x * sqrt(1.0 - (yy + zz) / 2.0 + (yy * zz) / 3);
    position.y = position.y * sqrt(1.0 - (zz + xx) / 2.0 + (zz * xx) / 3);
    position.z = position.z * sqrt(1.0 - (xx + yy) / 2.0 + (xx * yy) / 3);

    gl_Position = camera.projection * camera.transform * model.transform * vec4( position * radius, 1.0);
    gl_Position.y = -gl_Position.y;






//    _color = vec4(1);
//    switch(patch_vertices_id)
//    {
//        case 0: _color = vec4(1.f, 0.f, 0.f, 1.f); break;
//        case 1: _color = vec4(0.f, 1.f, 0.f, 1.f); break;
//        case 2: _color = vec4(.3f, .3f, 1.f, 1.f); break;
//        case 3: _color = vec4(1.f, 1.f, 0.f, 1.f); break;
//        case 4: _color = vec4(0.f, 1.f, 1.f, 1.f); break;
//        case 5: _color = vec4(1.f, 0.f, 1.f, 1.f); break;
//    }

}

