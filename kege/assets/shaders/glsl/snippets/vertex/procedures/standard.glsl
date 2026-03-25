vec3 processVertexPosition()
{
    return vec3( object.model_matrix * vec4(in_position, 1.0) );
}

vec3 processVertexNormal()
{
    return normalize(mat3(object.normal_matrix) * in_normal);
}

vec2 processVertexTexcoord()
{
    return in_texcoord;
}

