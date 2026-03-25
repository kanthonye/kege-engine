#version 450

struct DrawElem
{
    vec4  rect;
    vec4  texel;
    ivec4 border;
    vec4  clip_rect;
    ivec4 combo;
};

layout( set = 0, binding = 0 ) buffer UIViewBuffer
{
    DrawElem instances[];
};

layout( push_constant ) uniform Camera
{
    mat4 projection;
    vec4 resolution;
};

const vec2 vertex_position[ 4 ] = vec2[]
(
    vec2( 1.f, 0.f),
    vec2( 0.f, 0.f),
    vec2( 1.f,-1.f),
    vec2( 0.f,-1.f)
);

layout(location = 0) out vec4  color;
layout(location = 1) out vec4  rect;
layout(location = 2) out vec4  clip_rect;
layout(location = 3) out vec2  text_uv;
layout(location = 4) flat out  ivec2 text_info;
layout(location = 5) out vec4  border_radius;
layout(location = 6) out vec4  border_color;
layout(location = 7) out float border_width;

vec4 unpackRGBA8(uint packed)
{
    vec4 color;
    color.r = float((packed >> 24) & 0xFF);
    color.g = float((packed >> 16) & 0xFF);
    color.b = float((packed >> 8) & 0xFF);
    color.a = float(packed & 0xFF);

    return color / 255.0;
}

ivec2 unpackUint16x2(uint packed)
{
    return ivec2(
        int(packed & 0xFFFFu),
        int(packed >> 16)
    );
}

void main()
{
    clip_rect = instances[ gl_InstanceIndex ].clip_rect;
    color = unpackRGBA8( instances[ gl_InstanceIndex ].combo.r );
    text_info = unpackUint16x2( instances[ gl_InstanceIndex ].combo.g );
    rect = instances[ gl_InstanceIndex ].rect;

    border_radius.xy = vec2(unpackUint16x2( instances[ gl_InstanceIndex ].border.x ));
    border_radius.zw = vec2(unpackUint16x2( instances[ gl_InstanceIndex ].border.y ));
    border_color = unpackRGBA8( instances[ gl_InstanceIndex ].border.z );
    border_width = float(instances[ gl_InstanceIndex ].border.w);

    //color = instances[ gl_InstanceIndex ].color;
    //position = rect.xy;
    //size = rect.zw;

    vec4 texel = instances[ gl_InstanceIndex ].texel;
    text_uv = vertex_position[ gl_VertexIndex ] * vec2(1.f,-1.f);
    text_uv = texel.xy + texel.zw * text_uv;

    vec2 vp = vec2(rect.x, -rect.y) + rect.zw * vertex_position[ gl_VertexIndex ];
    gl_Position = projection * vec4( vp, 0.0f, 1.0f );
    gl_Position.y = -gl_Position.y;

    //border_radius = instances[ gl_InstanceIndex ].combo.x;
    //texture_id = instances[ gl_InstanceIndex ].combo.y;

}
