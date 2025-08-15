#version 450
layout(location = 0) in  vec3 _normal;
layout(location = 0) out vec4 out_color;

void main()
{
    float diff = 1.0;//max(0.4, dot(normalize(vec3(100,1000,400)), _normal));
    out_color = vec4( diff * vec3(0, 0.7, 0.9), 1 );
}
