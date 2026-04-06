#version 450

layout(location = 0) in vec4  color;
layout(location = 1) in vec4  rect;          // xy = pos, zw = size
layout(location = 2) in vec4  clip_rect;
layout(location = 3) in vec2  text_uv;
layout(location = 4) flat in ivec2 text_info;
layout(location = 5) in vec4  border_radius; // already unpacked vec4
layout(location = 6) in vec4  border_color;  // already unpacked RGBA
layout(location = 7) in float border_width;  // already unpacked (px)

layout(location = 0) out vec4 final_color;



// New texture sampler for UI textures
layout(set = 1, binding = 0) uniform sampler2D _theme;

// Texture samplers
layout(set = 2, binding = 0) uniform sampler2D _font;

// New texture sampler for UI textures
layout(set = 3, binding = 0) uniform sampler2D _scene;

// Camera uniform for 2D rendering
layout(push_constant) uniform Camera2D
{
    mat4 projection;
    vec4 resolution;
};



// p: fragment position relative to rect center
// size: full width/height
// r: vec4(TL, TR, BR, BL)
float sdRoundedRect(vec2 p, vec2 size, vec4 r)
{
    vec2 h = size * 0.5;

    // Determine quadrant
    bool right  = p.x > 0.0;
    bool bottom = p.y > 0.0;

    float radius =
        (!right && !bottom) ? r.x : // top-left
        ( right && !bottom) ? r.y : // top-right
        ( right &&  bottom) ? r.z : // bottom-right
                              r.w; // bottom-left

    vec2 q = abs(p) - h + vec2(radius);
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

vec4 sampleTexture(int texr_id)
{
    if (texr_id == 0) return texture(_theme, text_uv);
    if (texr_id == 1) return texture(_font,  text_uv);
    if (texr_id == 2) return texture(_scene, text_uv);
    return vec4(1.0);
}

void main()
{
    vec2 frag = gl_FragCoord.xy;

    // -----------------------------
    // Clip rectangle
    // -----------------------------
    if
    (
        frag.x < clip_rect.x || frag.x > clip_rect.x + clip_rect.z ||
        frag.y < clip_rect.y || frag.y > clip_rect.y + clip_rect.w
    )
    {
        discard;
    }

    // -----------------------------
    // Rectangle local space
    // -----------------------------
    vec2 size   = rect.zw;
    vec2 center = rect.xy + size * 0.5;
    vec2 p      = frag * vec2(1536.0, 896.0) - center;

    // Clamp radii to sane values
    vec4 r = min(border_radius, min(size.x, size.y) * 0.5);

    // -----------------------------
    // Signed distance
    // -----------------------------
    float d  = sdRoundedRect(p, size, r);
    float aa = fwidth(d);

    // Fill coverage (inside the shape)
    float fill_alpha = smoothstep( aa, -aa, d );
    vec3 rgb = color.rgb;

    // -----------------------------
    // Border coverage (band around edge)
    // -----------------------------
//    float border_alpha = smoothstep(
//        border_width + aa,
//        border_width - aa,
//        abs(d)
//    );
//
//    // Base color & alpha
//    float A = fill_alpha;
//
//    // Apply border on top of fill
//    rgb = mix(rgb, border_color.rgb, border_alpha);
//    fill_alpha = max(A, border_alpha);

    // -----------------------------
    // Text rendering (your logic)
    // -----------------------------
    int texr_id = text_info.x;

    vec4 texr_color = sampleTexture(texr_id);
    if (texr_id == 1) // font
    {
        float width = 0.05;
        float edge  = 0.07;

        float text_alpha = 1.0 - texr_color.r;
        text_alpha = 1.0 - smoothstep(width, width + edge, text_alpha);

        fill_alpha *= text_alpha;
    }
    else// if (texr_id >= 0)
    {
        rgb *= texr_color.rgb;
    }

    fill_alpha *= color.a;

    // -----------------------------
    // Final output
    // -----------------------------
    if (fill_alpha <= 0.0)
        discard;

//    final_color = vec4(rgb, A);
    final_color = vec4(rgb, fill_alpha);
}

