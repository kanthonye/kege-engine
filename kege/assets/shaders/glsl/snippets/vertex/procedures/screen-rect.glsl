vec2 positions[4] = vec2[]
(
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0, -1.0)
);

vec3 processVertexPosition()
{
    return vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
}
vec3 processVertexNormal()
{
    return vec3(0.0);
}
vec2 processVertexTexcoord()
{
    return (positions[ gl_VertexIndex ] + 1.0f) * 0.5;
}
