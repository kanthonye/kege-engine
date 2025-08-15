#version 450

const vec2 position[ 4 ] = vec2[]
(
    vec2( 1.f, 1.f),
    vec2(-1.f, 1.f),
    vec2( 1.f,-1.f),
    vec2(-1.f,-1.f)
);

layout(location = 0) out vec2 _texcoord;

void main()
{
    _texcoord = position[ gl_VertexIndex ] * 0.5 + 0.5;
    gl_Position = vec4( position[gl_VertexIndex], 0.0, 1.0 );
    //gl_Position.y = -gl_Position.y;
}

