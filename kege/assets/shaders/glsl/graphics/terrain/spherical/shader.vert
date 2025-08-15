#version 450

layout(set = 0, binding = 0) uniform CameraBlock
{
    mat4 projection;
    mat4 transform;
    vec3 position;
} camera;


struct PatchVertices
{
    vec3 positions[ 9 ];
};
layout( set = 1, binding = 0 ) uniform PatchVertexBuffer
{
    PatchVertices patch_vertices[ 6 ];
};


struct PatchIndices
{
    uint indices[ 24 ];
};
layout( set = 1, binding = 1 ) uniform PatchIndexBuffer
{
    PatchIndices patch_indices[ 16 ];
};


struct PatchData
{
    vec4 translation_scale;
    uint vertices_id;
    uint indices_id;
};
layout( std140, set = 2, binding = 0 ) buffer PatchDataBuffer
{
    PatchData patch_data[];
};



layout( push_constant ) uniform ObjectMatrices
{
    mat4  transform;
    float radius;
} model;


layout(location = 0) out vec4 _color;

void main()
{
    uint  patch_indices_id  = uint(patch_data[ gl_InstanceIndex ].indices_id);
    uint  patch_vertices_id = uint(patch_data[ gl_InstanceIndex ].vertices_id);
    vec4  translation_scale = patch_data[ gl_InstanceIndex ].translation_scale;
    vec3  translation  = translation_scale.xyz;
    float scale = translation_scale.w;
//
//
////    uint  vertex_index = patch_indices[ indices_id + gl_VertexIndex ];//.indices[ gl_VertexIndex ];
////    vec3  vertex_position = patch_vertices[ vertices_id + vertex_index ]yz;//.positions[ vertex_index ];
//    //float elevation = instances[ gl_InstanceIndex ].elevations[ vertex_index  ];
//
//
//    uint  vertex_index = patch_indices[ indices_id ].indices[ gl_VertexIndex ];
//    vec3  vertex_position = patch_vertices[ vertices_id ].positions[ vertex_index ];
////    float elevation = instances[ gl_InstanceIndex ].elevations[ vertex_index  ];
//
//    vec3 position = translation + 10 * vertex_position;
//    //vec3 position = translation + scale * vec3( vertex_position, 0.0, vertex_position.y );
//    vec3 normal = normalize( position );
//    position = normal;// * (model.radius + elevation);
//
//    vec4 transform_position = model.transform * vec4( normal, 1.0 );
//    normal = model.rotation * normal;

//    int data[24];
//    data[  0 ] =  0; // 0----1----2
//    data[  1 ] =  4; // |\       /|
//    data[  2 ] =  2; // | \     / |
//    data[  3 ] =  2; // |  \   /  |
//    data[  4 ] =  4; // |   \ /   |
//    data[  5 ] =  8; // 3    4    5
//    data[  6 ] =  8; // |   / \   |
//    data[  7 ] =  4; // |  /   \  |
//    data[  8 ] =  6; // | /     \ |
//    data[  9 ] =  6; // |/       \|
//    data[ 10 ] =  4; // 6----7----8
//    data[ 11 ] =  0;

    uint vertex_index = patch_indices[ patch_indices_id ].indices[ gl_VertexIndex ];
    vec3 position = patch_vertices[ patch_vertices_id ].positions[ vertex_index ];

    position = translation + position * scale;

    float xx = position.x * position.x;
    float yy = position.y * position.y;
    float zz = position.z * position.z;
    position.x = position.x * sqrt(1.0 - (yy + zz) / 2.0 + (yy * zz) / 3);
    position.y = position.y * sqrt(1.0 - (zz + xx) / 2.0 + (zz * xx) / 3);
    position.z = position.z * sqrt(1.0 - (xx + yy) / 2.0 + (xx * yy) / 3);

    position = model.radius * position;

    gl_Position = camera.projection * camera.transform * model.transform * vec4( position, 1.0);
    gl_Position.y = -gl_Position.y;

    switch(patch_vertices_id)
    {
        case 0: _color = vec4(1.f, 0.f, 0.f, 1.f); break;
        case 1: _color = vec4(0.f, 1.f, 0.f, 1.f); break;
        case 2: _color = vec4(.3f, .3f, 1.f, 1.f); break;
        case 3: _color = vec4(1.f, 1.f, 0.f, 1.f); break;
        case 4: _color = vec4(0.f, 1.f, 1.f, 1.f); break;
        case 5: _color = vec4(1.f, 0.f, 1.f, 1.f); break;
    }

}

