//
//  flat-terrain-tile-node.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/11/25.
//

#include "flat-terrain-tile-node.hpp"

namespace kege{

    void TerrainTileNode::setNeighborNorth( TerrainTileNode* node )
    {
        neighbor.north = node;
        if ( node == nullptr )
        {
            patch.index_buffer_id &= ~NORTH_VERTEX_BIT;
        }
        else
        {
            patch.index_buffer_id |= NORTH_VERTEX_BIT;
        }
    }
    
    void TerrainTileNode::setNeighborSouth( TerrainTileNode* node )
    {
        neighbor.south = node;
        if ( node == nullptr )
        {
            patch.index_buffer_id &= ~SOUTH_VERTEX_BIT;
        }
        else
        {
            patch.index_buffer_id |= SOUTH_VERTEX_BIT;
        }
    }

    void TerrainTileNode::setNeighborEast( TerrainTileNode* node )
    {
        neighbor.east = node;
        if ( node == nullptr )
        {
            patch.index_buffer_id &= ~EAST_VERTEX_BIT;
        }
        else
        {
            patch.index_buffer_id |= EAST_VERTEX_BIT;
        }
    }

    void TerrainTileNode::setNeighborWest( TerrainTileNode* node )
    {
        neighbor.west = node;
        if ( node == nullptr )
        {
            patch.index_buffer_id &= ~WEST_VERTEX_BIT;
        }
        else
        {
            patch.index_buffer_id |= WEST_VERTEX_BIT;
        }
    }

    TerrainTileNode::~TerrainTileNode()
    {
        if ( neighbor.north ) neighbor.north->setNeighborSouth( nullptr );
        if ( neighbor.south ) neighbor.south->setNeighborNorth( nullptr );
        if ( neighbor.east  ) neighbor.east->setNeighborWest( nullptr );
        if ( neighbor.west  ) neighbor.west->setNeighborEast( nullptr );
    }
}
