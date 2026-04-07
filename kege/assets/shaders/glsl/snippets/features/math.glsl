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
