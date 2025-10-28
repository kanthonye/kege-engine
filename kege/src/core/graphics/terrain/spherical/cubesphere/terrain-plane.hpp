//
//  terrain-plane.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/12/25.
//

#ifndef terrain_plane_hpp
#define terrain_plane_hpp

#include "terrain-tile.hpp"

namespace kege{

    struct TerrainPlane;
    
    struct TerrainPlaneNeighbors
    {
        TerrainPlane* north;
        TerrainPlane* south;
        TerrainPlane* east;
        TerrainPlane* west;
    };

    struct TerrainPlane
    {
        TerrainPlaneNeighbors neighbors;
        kege::fvec3 axies[ 3 ];
        TerrainTile root;

        const kege::vec4* vertices;
        int16_t face_id;
    };

}
#endif /* terrain_plane_hpp */
