//
//  flat-terrain-node.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/11/25.
//

#include "flat-terrain.hpp"
#include "flat-terrain-node.hpp"
#include "flat-terrain-tile.hpp"

namespace kege{

    void FlatTerrainQuadtree::operator()( FlatTerrainTile* tile, const dvec3& position, uint32_t diameter, uint32_t depth )
    {
        children   = nullptr;
        diameter   = diameter;
        center     = position;
        depth      = depth;

        patch.x = center.x;
        patch.y = center.z;
        patch.radius = diameter * 0.5;
        patch.index_buffer_id = 0;

        patch.image_index = tile->getImageLayer().imageIndex();
        patch.image_layer = tile->getImageLayer().imageLayer();
        patch.tile_position[0] = tile->getRoot().center[0];
        patch.tile_position[1] = tile->getRoot().center[2];

        patch.color = tile->color;

        local.x = (center.x - tile->getRoot().center.x) + tile->getRoot().patch.radius;
        local.y = (center.z - tile->getRoot().center.z) + tile->getRoot().patch.radius;

        tile->setHeight( *this );
    }

    void FlatTerrainQuadtree::split( FlatTerrainTile* tile )
    {
        uint32_t child_width  = diameter * 0.5;
        uint32_t child_offset = child_width * 0.5;
        uint32_t child_depth  = depth  + 1;

        dvec3 centers[4];

        centers[0].x = center.x - child_offset;
        centers[0].z = center.z + child_offset;
        centers[0].y = center.y;

        centers[1].x = center.x + child_offset;
        centers[1].z = center.z + child_offset;
        centers[1].y = center.y;

        centers[2].x = center.x - child_offset;
        centers[2].z = center.z - child_offset;
        centers[2].y = center.y;

        centers[3].x = center.x + child_offset;
        centers[3].z = center.z - child_offset;
        centers[3].y = center.y;

        children = new FlatTerrainQuadtreeChildren;
        children->nw.operator()( tile, centers[0], child_width, child_depth );
        children->ne.operator()( tile, centers[1], child_width, child_depth );
        children->sw.operator()( tile, centers[2], child_width, child_depth );
        children->se.operator()( tile, centers[3], child_width, child_depth );

        /*
         A child descendent can only have neighbors if and only if that neighbor and
         the child are at the same depth. This is a crucial step in correcting the
         disuniform LOD between terrain nodes. If a terrain tile node's adjacent
         neighbor is null, then the edge center vertex between the terrain tile node
         and its adjacent null neighbor is disabled.
         */

        children->nw.setNeighborEast( &children->ne );
        children->ne.setNeighborWest( &children->nw );
        children->sw.setNeighborEast( &children->se );
        children->se.setNeighborWest( &children->sw );

        children->nw.setNeighborSouth( &children->sw );
        children->sw.setNeighborNorth( &children->nw );
        children->ne.setNeighborSouth( &children->se );
        children->se.setNeighborNorth( &children->ne );

        if ( neighbor.north )
        {
            if( neighbor.north->children )
            {
                neighbor.north->children->sw.setNeighborSouth( &children->nw );
                children->nw.setNeighborNorth( &neighbor.north->children->sw );

                neighbor.north->children->se.setNeighborSouth( &children->ne );
                children->ne.setNeighborNorth( &neighbor.north->children->se );
            }
        }
        if ( neighbor.south )
        {
            if( neighbor.south->children )
            {
                neighbor.south->children->nw.setNeighborNorth( &children->sw );
                children->sw.setNeighborSouth( &neighbor.south->children->nw );

                neighbor.south->children->ne.setNeighborNorth( &children->se );
                children->se.setNeighborSouth( &neighbor.south->children->ne );
            }
        }
        if ( neighbor.east )
        {
            if( neighbor.east->children )
            {
                neighbor.east->children->nw.setNeighborWest( &children->ne );
                children->ne.setNeighborEast( &neighbor.east->children->nw );

                neighbor.east->children->sw.setNeighborWest( &children->se );
                children->se.setNeighborEast( &neighbor.east->children->sw );
            }
        }
        if ( neighbor.west )
        {
            if( neighbor.west->children )
            {
                neighbor.west->children->ne.setNeighborEast( &children->nw );
                children->nw.setNeighborWest( &neighbor.west->children->ne );

                neighbor.west->children->se.setNeighborEast( &children->sw );
                children->sw.setNeighborWest( &neighbor.west->children->se );
            }
        }
    }

    void FlatTerrainQuadtree::setNeighborNorth( FlatTerrainQuadtree* node )
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

    void FlatTerrainQuadtree::setNeighborSouth( FlatTerrainQuadtree* node )
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

    void FlatTerrainQuadtree::setNeighborEast( FlatTerrainQuadtree* node )
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

    void FlatTerrainQuadtree::setNeighborWest( FlatTerrainQuadtree* node )
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

    void FlatTerrainQuadtree::merge()
    {
        if ( children )
        {
            children->nw.merge();
            children->ne.merge();
            children->sw.merge();
            children->se.merge();

            delete children;
            children = nullptr;
        }
    }

    FlatTerrainQuadtree::~FlatTerrainQuadtree()
    {
        if ( neighbor.north ) neighbor.north->setNeighborSouth( nullptr );
        if ( neighbor.south ) neighbor.south->setNeighborNorth( nullptr );
        if ( neighbor.east  ) neighbor.east->setNeighborWest( nullptr );
        if ( neighbor.west  ) neighbor.west->setNeighborEast( nullptr );
        merge();
    }

}
