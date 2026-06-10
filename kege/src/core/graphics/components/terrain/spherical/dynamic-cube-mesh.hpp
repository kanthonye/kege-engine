//
//  dynamic-cube-mesh.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef dynamic_cube_mesh_hpp
#define dynamic_cube_mesh_hpp

#include "../common/all.hpp"

namespace kege{

    /**
     * This struct represents the vertex data for a single face of a cube. Each
     * face is composed of 8 triangles max, requiring a total of nine vertices,
     * abd 24 indices when rendered at full resolution.
     */
    struct PatchVertices // PatchVertices
    {
        kege::vec4 data[ 9 ];
    };

    /**
     * This struct represents the index data for a single face of a cube. It
     * specifies the order in which vertices from the PatchVertices struct
     * should be connected to form triangles. At max resolution 24 indices are
     * used, rendering 9 triangles.
     */
    struct PatchIndices // PatchIndices
    {
        uint32_t data[ 24 ];
        uint16_t draw_count;
    };


    struct PatchData
    {
        PatchData();

        /**
         * The transformation data for this patch. It's xyz components represents
         * the position and the w component represents the scale
         */
        kege::fvec4 transform;
    };

}

namespace kege{

    enum FaceType
    {
        CUBE_FACE_FRONT,
        CUBE_FACE_BACK,
        CUBE_FACE_LEFT,
        CUBE_FACE_RIGHT,
        CUBE_FACE_ABOVE,
        CUBE_FACE_BELOW,
        MAX_CUBE_FACES
    };

    /**
     * @brief Represents the root node of a single quadtree plane.
     */
    struct DynamicCubeMesh
    {
        DynamicCubeMesh( float scale );
        DynamicCubeMesh();

        /**
         * This struct represents the vertex data for all 6 faces that make up the
         * cube. Each face is composed of 9 vertices, that makes a max of 8 triangles,
         */
        PatchVertices face_vertices[ MAX_CUBE_FACES ];

        /**
         * Each face corespond to a set of 3 axes. These 3 axes for each face are the
         * x,y and z axes.
         */
        kege::mat33 face_axies[ MAX_CUBE_FACES ];

        /**
         * Each PatchIndices struct contains index data for a specific combination
         * of triangles, allowing for smooth transitions between LODs.
         */
        PatchIndices face_indices[ 16 ];
    };

}
#endif /* dynamic_cube_mesh_hpp */
