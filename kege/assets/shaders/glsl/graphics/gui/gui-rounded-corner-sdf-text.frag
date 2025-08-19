#version 450
layout(location = 0) out vec4 final_color;

// Input attributes for rectangle and text rendering
layout(location = 0) in vec4 color;
layout(location = 1) in vec2 position;
layout(location = 2) in vec2 size;
layout(location = 3) in float border_radius;
layout(location = 4) in vec4 clip_rect;
layout(location = 5) in vec2 text_uv;
layout(location = 6) in float render_text;
layout(location = 7) in float texture_id;   // Texture array index

// Texture samplers
layout(set = 1, binding = 0) uniform sampler2D sdf_font_texture;

#define MAX_TEXTURES 16 // Define a maximum number of textures

// New texture sampler for UI textures
layout(set = 2, binding = 0) uniform sampler2D ui_textures[MAX_TEXTURES]; // Define MAX_TEXTURES as needed

// Camera uniform for 2D rendering
layout(push_constant) uniform Camera2D {
    mat4 projection;
    vec4 resolution;
};

// Function to compute the signed distance field for a rounded rectangle
float roundedBoxSDF(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

void main() {
    // Normalize fragment coordinates
    vec2 frag_coord = gl_FragCoord.xy * resolution.z;

    // Clip rectangle check
    if(frag_coord.x < clip_rect.x || frag_coord.x > clip_rect.x + clip_rect.z ||
       frag_coord.y < clip_rect.y || frag_coord.y > clip_rect.y + clip_rect.w) {
        discard;
    }

    frag_coord = frag_coord / resolution.xy;

    // Existing rectangle SDF calculation
    // Compute pixel position in rectangle space
    vec2 half_size = size * 0.5;
    vec2 center = position + half_size;
    vec2 rect_space = frag_coord * resolution.xy - center;
    float sdf = roundedBoxSDF(rect_space, half_size, border_radius);
    float alpha = smoothstep(0.0, 1.0, -sdf);

    vec4 final;
    if ( texture_id >= 0 && texture_id <= 15 )
    {
        // Could use texture arrays or switch-case for multiple textures
        final = color * texture( ui_textures[ int( texture_id) ], text_uv );
    }
    else
    {
        final = color;
    }

    if ( render_text >= 1.0 )
    {
        float width = 0.05;
        float edge  = 0.07;

        // Text rendering: Sample from the SDF font texture
        float text_alpha = 1 - texture(sdf_font_texture, text_uv).r;

        // Apply smoothstep for anti-aliased text
        alpha = 1 - smoothstep(width, width + edge, text_alpha);
    }

    final_color = vec4( final.rgb, final.a * alpha );
}


//#version 450
//layout(location = 0) out vec4 final_color;
//
//layout(location = 0) in vec4  color;   // Rectangle color
//layout(location = 1) in vec2  position; // Rectangle position (bottom-left)
//layout(location = 2) in vec2  size;     // Rectangle size
//layout(location = 3) in float border_radius; // border radius size
//layout(location = 4) in vec4  clip_rect;
//layout(location = 5) in vec2  text_uv;  // UV coordinates for text rendering
//layout(location = 6) in float use_text; // Flag to determine if text should render
//
//layout(set = 1, binding  = 0) uniform sampler2D sdf_font_texture; // SDF text texture
//layout(set = 2, binding  = 0) uniform sampler2D theme; // SDF text texture
//
//layout( push_constant ) uniform Camera2D
//{
//    mat4 projection;
//    vec4 resolution;
//};
//
//
//float roundedBoxSDF(vec2 p, vec2 b, float r) {
//    vec2 q = abs(p) - b + r;
//    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
//}
//
//void main() {
//
//    vec2 frag_coord = gl_FragCoord.xy * resolution.z;
//
//    if
//    (
//        frag_coord.x < clip_rect.x || frag_coord.x > clip_rect.x + clip_rect.z ||
//        frag_coord.y < clip_rect.y || frag_coord.y > clip_rect.y + clip_rect.w
//    )
//    {
//        discard; // Discard the fragment
//    }
//
//    frag_coord = frag_coord / resolution.xy;
//
//    // Normalize pixel coordinates to [0, 1] range
//    // Compute pixel position in rectangle space
//    vec2 half_size = size * 0.5;
//
//    // Center the rectangle at the origin
//    vec2 center = position + half_size;
//
//    // Convert to rectangle space (center at origin)
//    vec2 rect_space = frag_coord * resolution.xy - center;
//
//    // Compute Signed Distance Field (SDF) value for rounded rectangle
//    float sdf = roundedBoxSDF(rect_space, half_size, border_radius);
//
//    // Compute alpha for rounded rectangle
//    float rect_alpha = smoothstep(0.0, 1.0, -sdf);
//
//    float width       = 0.05;
//    float edge        = 0.07;
//
//    // Text rendering: Sample from the SDF font texture
//    float text_alpha = 1 - texture(sdf_font_texture, text_uv).r;
//
//    // Apply smoothstep for anti-aliased text
//    text_alpha = 1 - smoothstep(width, width + edge, text_alpha);
//
//    // Blend text with rectangle if enabled
//    float alpha = rect_alpha;
//    if (use_text > 0.5)
//    {
//        alpha = text_alpha; // Multiply alpha to mask text inside the rectangle
//    }
//
//
//    // Output final color
//    final_color = vec4(color.rgb, color.a * alpha);
//}
