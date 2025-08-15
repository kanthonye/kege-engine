#version 450

//layout( push_constant ) uniform LightColor
//{
//    vec3 color;
//};

layout(location = 0) in  vec2 _offsets;
layout(location = 1) in  vec4 _color;
layout(location = 0) out vec4 out_color;

void main()
{
    float dist = sqrt( dot(_offsets, _offsets) );
    if ( dist >= 1 ) {
        discard;
    }
    out_color = _color;//vec4( 1 );
}
