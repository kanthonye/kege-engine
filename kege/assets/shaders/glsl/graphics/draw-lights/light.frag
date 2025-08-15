#version 450

//layout( push_constant ) uniform LightColor
//{
//    vec3 color;
//};

layout(location = 0) in  vec2 _vertex_position;
layout(location = 0) out vec4 out_color;

void main()
{
    float dist = sqrt( dot(_vertex_position, _vertex_position) );
    if ( dist >= 1 ) {
        discard;
    }
    out_color = vec4( 1 );
}
