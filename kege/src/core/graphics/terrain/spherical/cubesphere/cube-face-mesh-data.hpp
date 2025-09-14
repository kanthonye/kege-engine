//
//  patch-indice-array.hpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/5/24.
//

#ifndef patch_indice_array_hpp
#define patch_indice_array_hpp

#include "dynamic-cube-mesh.hpp"


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
}
#endif /* patch_indice_array_hpp */
