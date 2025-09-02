//
//  dynamic-cube-mesh.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#ifndef dynamic_cube_mesh_hpp
#define dynamic_cube_mesh_hpp

#include "cube-face-mesh-data.hpp"

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
