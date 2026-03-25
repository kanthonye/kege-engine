vec4 processVertexPosition()
{
    float size = in_extras.x;
    float rotation = in_extras.y;

    // Extract the right and up vectors from the view matrix
    vec3 camera_right = vec3(camera.transform[0][0], camera.transform[1][0], camera.transform[2][0]);
    vec3 camera_up    = vec3(camera.transform[0][1], camera.transform[1][1], camera.transform[2][1]);

    // Calculate the offset from the particle position in world space
    vec2 quad_point = vertex_position[ gl_VertexIndex ] * size * 0.5;

    // Calculate the world position of the billboard vertex
    vec3 billboard_world_position = in_position.xyz + camera_right * quad_point.x + camera_up * quad_point.y;
    return object.model_matrix * vec4( billboard_world_position, 1.0 );
}

vec3 processVertexNormal()
{
    return vec2(0);
}
vec2 processVertexTexcoord()
{
    return vec2(0);
}
vec4 processVertexColor()
{
    return in_color;
}
