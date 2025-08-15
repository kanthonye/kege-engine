#version 450

layout(location = 0) in vec4 _color;

layout(location = 0) out vec4 _final_color;
void main()
{
    _final_color = _color;
}



