//
//  spherical-terrain-tile.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/4/25.
//

#include "spherical-terrain-tile.hpp"
#include "physical-spherical-terrain.hpp"

namespace kege{

    SphericalTerrainTile::SphericalTerrainTile()
    :   children( nullptr )
    ,   depth( 0 )
    ,   visible( true )
    {}

}
