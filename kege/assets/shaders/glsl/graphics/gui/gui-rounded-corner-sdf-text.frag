#version 450
layout(location = 0) out vec4 final_color;

layout(location = 0) in vec4  color;   // Rectangle color
layout(location = 1) in vec2  position; // Rectangle position (bottom-left)
layout(location = 2) in vec2  size;     // Rectangle size
layout(location = 3) in float border_radius; // border radius size
layout(location = 4) in vec4  clip_rect;

layout(set = 1, binding  = 0) uniform sampler2D sdfFontTexture; // SDF text texture
layout(location = 5) in vec2  text_uv;  // UV coordinates for text rendering
layout(location = 6) in float use_text; // Flag to determine if text should render


layout( push_constant ) uniform Camera2D
{
    mat4 projection;
    vec4 resolution;
};


float roundedBoxSDF(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

void main() {

    vec2 frag_coord = gl_FragCoord.xy * resolution.z;

    if
    (
        frag_coord.x < clip_rect.x || frag_coord.x > clip_rect.x + clip_rect.z ||
        frag_coord.y < clip_rect.y || frag_coord.y > clip_rect.y + clip_rect.w
    )
    {
        discard; // Discard the fragment
    }

    frag_coord = frag_coord / resolution.xy;

    // Normalize pixel coordinates to [0, 1] range
    // Compute pixel position in rectangle space
    vec2 half_size = size * 0.5;

    // Center the rectangle at the origin
    vec2 center = position + half_size;

    // Convert to rectangle space (center at origin)
    vec2 rect_space = frag_coord * resolution.xy - center;

    // Compute Signed Distance Field (SDF) value for rounded rectangle
    float sdf = roundedBoxSDF(rect_space, half_size, border_radius);

    // Compute alpha for rounded rectangle
    float rect_alpha = smoothstep(0.0, 1.0, -sdf);

    float width       = 0.05;
    float edge        = 0.07;

    // Text rendering: Sample from the SDF font texture
    float text_alpha = 1 - texture(sdfFontTexture, text_uv).r;

    // Apply smoothstep for anti-aliased text
    text_alpha = 1 - smoothstep(width, width + edge, text_alpha);

    // Blend text with rectangle if enabled
    float alpha = rect_alpha;
    if (use_text > 0.5)
    {
        alpha = text_alpha; // Multiply alpha to mask text inside the rectangle
    }


    // Output final color
    final_color = vec4(color.rgb, color.a * alpha);
}
