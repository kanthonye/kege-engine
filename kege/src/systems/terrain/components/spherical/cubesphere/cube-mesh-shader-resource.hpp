//
//  cube-mesh-shader-resource.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/9/24.
//

#ifndef cube_mesh_shader_resource_hpp
#define cube_mesh_shader_resource_hpp

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
    struct CubeMeshShaderResource
    {
        /**
         * This ShaderResourceLayout will store the mesh vertices and indices
         */
        kege::ShaderResourceLayout mesh_shader_resource;

        /**
         * This uniform buffer will store the mesh vertices
         */
        kege::UBO composite_vertices_uniform_buffer;

        /**
         * This uniform buffer will store the mesh indices
         */
        kege::UBO composite_indices_uniform_buffer;

        /**
         * This struct represents the vertex data for all 6 faces that make up the
         * cube. Each face is composed of 9 vertices, that makes a max of 8 triangles,
         */
        PatchVertices composite_vertices[ MAX_CUBE_FACES ];

        /**
         * Each PatchIndices struct contains index data for a specific combination
         * of triangles, allowing for smooth transitions between LODs.
         */
        PatchIndices composite_indices[ 16 ];

        /**
         * Each face corespond to a set of 3 axes. These 3 axes for each face are the
         * x,y and z axes.
         */
        kege::mat33 axies[ MAX_CUBE_FACES ];
        bool initialized;
    };
    
    const CubeMeshShaderResource* createQuadtreeCube( float scale );

}
#endif /* cube_mesh_shader_resource_hpp */
