#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 invViewProj;
    vec3 cameraPos;
    float time;
} camera;

float grid(vec3 worldPos, float scale) {
    vec2 coord = worldPos.xz / scale;
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    float line = min(grid.x, grid.y);
    return 1.0 - min(line, 1.0);
}

void main() {
    // Reconstruct world pos from screen space
    vec2 ndc = fragUV * 2.0 - 1.0;
    vec4 clipPos = vec4(ndc, 1.0, 1.0);
    vec4 worldPos = camera.invViewProj * clipPos;
    worldPos /= worldPos.w;

    // Pick scale based on distance
    float dist = length(camera.cameraPos - worldPos.xyz);
    float scale = pow(2.0, floor(log2(dist * 0.25)));

    // Compose grid
    float fine = grid(worldPos.xyz, scale);
    float coarse = grid(worldPos.xyz, scale * 10.0);

    vec3 gridColor = mix(vec3(0.15), vec3(0.3), coarse);
    float alpha = max(fine, coarse);

    // Optional fade with distance
    float fade = clamp(1.0 - dist * 0.002, 0.0, 1.0);

    outColor = vec4(gridColor, alpha * fade);
}
