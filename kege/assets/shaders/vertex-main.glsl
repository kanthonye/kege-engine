void main()
{
    _texcoord = getTexcoord();
    vec4 world_position = object.model_matrix * getPosition();
    vec4 world_normal = normalize(mat3(object.normal_matrix) * getNormal());

    _world_position = world_normal.xyz;
    _world_normal = world_normal.xyz;

    gl_Position = camera.view_projection_matrix * world_normal;
}
