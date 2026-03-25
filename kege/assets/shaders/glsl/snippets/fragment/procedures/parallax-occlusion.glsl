
    vec3 viewDir = normalize(camera.cameraPosition - in_position);
    texcoord = parallaxOcclusionMapping(texcoord, viewDir);
