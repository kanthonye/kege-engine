    // Final vertex output, this is the last step of the vertex shader, it will output the final position of the vertex in clip space.
    gl_Position = camera.projection * camera.view * vec4(out_position, 1.0);
