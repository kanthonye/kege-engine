#version 450

const vec2 position[ 3 ] = vec2[]
(
    vec2( 0.f, 1.f),
    vec2( 1.f,-1.f),
    vec2(-1.f,-1.f)
);

void main()
{
    gl_Position = vec4( position[gl_VertexIndex], 0.0, 1.0 );
    gl_Position.y = -gl_Position.y;
}

