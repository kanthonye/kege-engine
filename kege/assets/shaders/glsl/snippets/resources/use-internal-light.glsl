    // Integrate the directional lights in the scene using the PBR lighting model
    lighting = pbrLighting
    (
        normal,
        view,
        vec3(0.707, 0.707, 0.707),
        vec3(1.0),
        albedo,
        metallic,
        roughness,
        ao
    );
    final_color = lighting + emissive;
