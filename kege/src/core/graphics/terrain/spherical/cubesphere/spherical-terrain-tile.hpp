//
//  spherical-terrain-tile.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/4/25.
//

#ifndef spherical_terrain_tile_hpp
#define spherical_terrain_tile_hpp

#include "cube-face-mesh-data.hpp"

namespace kege{

    class SphericalTerrain;
    struct SphericalTerrainTileChildren;


    /**
     * @brief A struct for storing a quadtree-based spatial data structure.
     */
    struct SphericalTerrainTile
    {
        SphericalTerrainTile();

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
        kege::SphericalTerrainTileChildren* children;

        /**
         * Hold the depth of this node.
         */
        int16_t depth;
        int16_t index_buffer_id;
        int16_t face_id;
        bool visible;
    };


    /**
     * @brief Contains the child nodes of the QuadtreePatchNode
     */
    struct SphericalTerrainTileChildren
    {
        SphericalTerrainTile nodes[ 4 ];
    };

}
#endif /* spherical_terrain_tile_hpp */
