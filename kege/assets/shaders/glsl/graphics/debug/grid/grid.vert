#version 450

layout(location = 0) out vec2 fragUV;

void main() {
    const vec2 pos[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );

    fragUV = pos[gl_VertexIndex].xy * 0.5 + 0.5;
    gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);
}
