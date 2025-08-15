#version 450
layout(location = 0) out vec4 frag_Color;
layout(location = 0) in  vec4 _color;

void main()
{
    frag_Color = vec4( _color );
}

