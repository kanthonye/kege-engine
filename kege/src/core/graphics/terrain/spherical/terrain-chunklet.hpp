//
//  terrain-chunklet.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/12/25.
//

#ifndef terrain_chunklet_hpp
#define terrain_chunklet_hpp

#include "../common/all.hpp"

namespace kege{

    struct TerrainChucklet;
    struct TerrainChucklets;
    struct SphericalTerrain;

    enum { NW, NE, SW, SE, MAX_CHILD_COUNT };
    enum { NORTH, EAST, SOUTH, WEST };

    struct TerrainChuckletNeighbors
    {
        TerrainChucklet* north;
        TerrainChucklet* south;
        TerrainChucklet* east;
        TerrainChucklet* west;
    };

    struct TerrainChucklet
    {
        TerrainChuckletNeighbors neighbors;
        TerrainChucklets* children;

        /**
         * The spherical volume that encapsulate this quadtree patch node. This
         * member is necessary for culling and computing the level of detail of
         * this QuadtreePatchNode.
         */
        kege::fvec4 sphere;

        /**
         * The PatchData to load into the render buffer when rendering this patch.
         */
        kege::vec4 quad;

        /**
         * The face normal of this QuadtreePatchNode.
         */
        kege::fvec3 normal;

        int16_t index_buffer_id;
        int16_t depth;
        int16_t face_id;

        bool visible;
    };
    
    struct TerrainChucklets
    {
        TerrainChucklet chucklets[ MAX_CHILD_COUNT ];
    };


}

namespace kege{ // TerrainChucklet

    void initChucklet
    (
        SphericalTerrain* terrain,
        TerrainChucklet& chunklet,
        const kege::vec3& position,
        float scale,
        float radius,
        int depth,
        int face_id
    );

    void submitVisibleChunklet( SphericalTerrain* terrain, TerrainChucklet& chunklet );
    bool chuckletIsSplitable( SphericalTerrain* terrain, TerrainChucklet& chucklet );
    bool chuckletIsDivisible( SphericalTerrain* terrain, TerrainChucklet& chucklet );
    void subdivideChucklet( SphericalTerrain* terrain, TerrainChucklet& chucklet );
    void updateChucklet( SphericalTerrain* terrain, TerrainChucklet& chucklet );
    void mergeChucklet( TerrainChucklet& chucklet );
}



#endif /* terrain_chunklet_hpp */
