#version 450

layout(location = 0) out vec4 out_color;

// If you have a global time uniform, you can use it to make it flicker
// layout(set = 0, binding = 1) uniform TimeBlock { float time; } t;

void main() {
    // Creates a 10x10 checkerboard pattern based on screen coordinates
    bool checker = bool((int(gl_FragCoord.x) / 10 + int(gl_FragCoord.y) / 10) % 2);

    vec3 color1 = vec3(1.0, 0.0, 1.0); // Magenta
    vec3 color2 = vec3(1.0, 0.0, 0.0); // Black

    out_color = vec4(checker ? color1 : color2, 1.0);
}
