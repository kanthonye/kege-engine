//
//  spherical-terrain-face.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/4/25.
//

#ifndef spherical_terrain_face_hpp
#define spherical_terrain_face_hpp

#include "spherical-terrain-tile.hpp"

namespace kege{

    /**
     * @brief Represents the root node of a single quadtree plane.
     */
    class SphericalTerrainFace
    {
        enum { NW, NE, SW, SE, MAX_CHILD_COUNT };
        enum { NORTH, EAST, SOUTH, WEST };

        void init
        (
            int16_t face_id,
            SphericalTerrain* terrain,
            SphericalTerrainTile* neighbors[4],
            const kege::vec4* vertices,
            const kege::mat33& axes,
            float scale,
            double radius,
            int depth
        );

        void operator()( SphericalTerrainTile& node, const kege::vec3& position, float scale, double radius, int depth );

        /**
         * @fn update
         *
         * Update the geometric surface
         *
         * @param node The current node to update
         */
        void update( SphericalTerrainTile& node, SphericalTerrainTile* neighbors[4] );
        void update();

        /**
         * @fn canSubDivide
         *
         * Test whether a node can be sub-divided. This function uses distance and size to determine the result.
         *
         * @param node The node to test
         * @return true if the node is too large or false if it is not.
         *
         */
        bool canSubDivide( SphericalTerrainTile& node );

        /**
         * @fn splitable
         *
         * Test whether a node can be split or not. This function uses the depth and children to determine the result.
         *
         * @param node The node to test
         * @return true if the node can be split or false if it can not.
         */
        bool splitable( SphericalTerrainTile& node );

        /**
         * @fn split
         *
         * Splits the quadtree node into smaller quadtree node.
         *
         * @param node The quadtree node to be split.
         */
        void split( SphericalTerrainTile& node );

        /**
         * @fn merge
         *
         * Merge the quadtree node child nodes into the parent quadtree node.
         *
         * @param node The quadtree node to be merge.
         */
        void merge( SphericalTerrainTile& node );

        SphericalTerrainTile _root;
        kege::vec3 _surface_axies[ 2 ];
        const kege::vec4* _vertices;

        SphericalTerrain* _terrain;
        SphericalTerrainTile* _neighbors[4];
        int16_t _face_id;

        friend SphericalTerrain;
    };

}
#endif /* spherical_terrain_face_hpp */
