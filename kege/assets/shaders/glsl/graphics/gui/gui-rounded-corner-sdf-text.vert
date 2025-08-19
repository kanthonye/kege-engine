#version 450

struct DrawElem
{
    vec4 rect;
    vec4 color;
    vec4 texel;
    vec4 clip_rect;
    vec4 combo;
};

layout( set = 0, binding = 0 ) buffer UIViewBuffer
{
    DrawElem instances[];
};

layout( push_constant ) uniform Camera2D
{
    mat4 projection;
    vec4 resolution;
};

const vec2 vertex_position[ 4 ] = vec2[]
(
    vec2( 0.f, 0.f),
    vec2( 1.f, 0.f),
    vec2( 0.f,-1.f),
    vec2( 1.f,-1.f)
);


layout(location = 0) out vec4  color;
layout(location = 1) out vec2  position;   // Position of the rectangle center
layout(location = 2) out vec2  size;       // Size of the rectangle
layout(location = 3) out float border_radius; // border radius size
layout(location = 4) out vec4  clip_rect;
layout(location = 5) out vec2  text_uv;  // UV coordinates for text rendering
layout(location = 6) out float render_text; // Flag to determine if text should render
layout(location = 7) out float texture_id;   // Texture array index

void main()
{
    clip_rect = instances[ gl_InstanceIndex ].clip_rect;

    vec4 rect = instances[ gl_InstanceIndex ].rect;
    color = instances[ gl_InstanceIndex ].color;
    position = rect.xy;
    size = rect.zw;

    vec2 vp = vec2(rect.x, -rect.y) + size * vertex_position[ gl_VertexIndex ];
    gl_Position = projection * vec4( vp, 0.0f, 1.0f );
    gl_Position.y = -gl_Position.y;

    vec4 texel = instances[ gl_InstanceIndex ].texel;
    border_radius = instances[ gl_InstanceIndex ].combo.x;
    render_text = instances[ gl_InstanceIndex ].combo.y;
    texture_id = instances[ gl_InstanceIndex ].combo.z;
    text_uv = vertex_position[ gl_VertexIndex ] * vec2(1.f,-1.f);
    text_uv = texel.xy + texel.zw * text_uv;

}
