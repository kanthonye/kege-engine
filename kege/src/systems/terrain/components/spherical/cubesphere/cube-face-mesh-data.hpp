//
//  patch-indice-array.hpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/5/24.
//

#ifndef patch_indice_array_hpp
#define patch_indice_array_hpp

#include "../../../../-/system-dependencies.hpp"

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
         * Heightmap elevations data for a single patch.
         */
//        float elevations[ 9 ];

        /**
         * The transformation data for this patch. It's xyz components represents
         * the position and the w component represents the scale
         */
        kege::fvec4 transform;

        /**
         * The patch_vertex_id hold the index to which PatchVertices to use when 
         * rendering this patch.
         */
        uint32_t patch_vertex_id;

        /**
         * The patch_index_id hold the index to which PatchIndices to use when 
         * rendering this patch.
         */
        uint32_t patch_index_id;


        uint32_t pddn[2];
    };

    struct PatchDrawBuffer
    {
        kege::DrawParamBuffer draw_buffer;
        kege::ShaderResourceLayout descriptor_set;
        kege::StorageBuffer patch_buffer;
        uint32_t instance_count;
    };

}


namespace kege{

    struct QuadtreePatchChildren;


    /**
     * @brief A struct for storing a quadtree-based spatial data structure.
     */
    struct QuadtreePatchNode
    {
        QuadtreePatchNode();
        
        /**
         * The PatchData to load into the render buffer when rendering this patch.
         */
        PatchData patch;

        /**
         * The spherical volume that encapsulate this quadtree patch node. This 
         * member is necessary for culling and computing the level of detail of
         * this QuadtreePatchNode.
         */
        kege::dvec4 sphere;

        /**
         * The face normal of this QuadtreePatchNode.
         */
        kege::fvec3 normal;

        /**
         * The child node of this QuadtreePatchNode.
         */
        kege::QuadtreePatchChildren* children;

        /**
         * Hold the depth of this node.
         */
        int16_t depth;
        bool visible;
    };


    /**
     * @brief Contains the child nodes of the QuadtreePatchNode
     */
    struct QuadtreePatchChildren
    {
        QuadtreePatchNode nodes[ 4 ];
    };


    /**
     * @brief Represents the root node of a single quadtree plane.
     */
    struct QuadtreePatch
    {
        QuadtreePatchNode root;
        const PatchVertices* vertices;
        kege::vec3 surface_axies[ 2 ];
    };


    uint32_t getCapSize( uint32_t count, uint32_t base, uint32_t exponent );
}
#endif /* patch_indice_array_hpp */
