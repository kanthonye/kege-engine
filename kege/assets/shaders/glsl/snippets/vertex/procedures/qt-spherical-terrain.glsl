struct PatchData
{
    vec4 translation_scale;
};
layout( std140, set = 1, binding = 0 ) buffer PatchDataBuffer
{
    PatchData patch_data[];
};

vec3 normal;
vec4 processVertexPosition()
{
    float radius  = object.rotation[3][0];
    float scale = patch_data[ gl_InstanceIndex ].translation_scale.w;
    vec3 translation  = patch_data[ gl_InstanceIndex ].translation_scale.xyz;
    vec3 position = translation + in_position.xyz * scale;

    float xx = position.x * position.x;
    float yy = position.y * position.y;
    float zz = position.z * position.z;
    normal.x = position.x * sqrt(1.0 - (yy + zz) / 2.0 + (yy * zz) / 3);
    normal.y = position.y * sqrt(1.0 - (zz + xx) / 2.0 + (zz * xx) / 3);
    normal.z = position.z * sqrt(1.0 - (xx + yy) / 2.0 + (xx * yy) / 3);

    return model.transform * vec4( normal * radius, 1.0);
}
vec3 processVertexNormal()
{
    return normal;
}
vec2 processVertexTexcoord()
{
    return (positions[ gl_VertexIndex ] + 1.0f) * 0.5;
}
