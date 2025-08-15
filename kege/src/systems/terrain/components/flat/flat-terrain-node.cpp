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

    void TerrainNodeChildren::render( FlatTerrainRenderer& renderer )
    {
        nodes[ 0 ].render( renderer );
        nodes[ 1 ].render( renderer );
        nodes[ 2 ].render( renderer );
        nodes[ 3 ].render( renderer );
    }

    void TerrainNodeChildren::update( const dvec3& eye )
    {
        nodes[ 0 ].update( eye );
        nodes[ 1 ].update( eye );
        nodes[ 2 ].update( eye );
        nodes[ 3 ].update( eye );
    }
    
    void TerrainNodeChildren::merge()
    {
        nodes[0].merge();
        nodes[1].merge();
        nodes[2].merge();
        nodes[3].merge();
    }

    TerrainNodeChildren::TerrainNodeChildren( FlatTerrain* terrain, const FlatTerrainNode* node )
    {
        uint32_t diameter  = node->diameter * 0.5;
        uint32_t offset = diameter * 0.5;
        uint32_t depth  = node->depth  + 1;

        dvec3 centers[4];

        centers[0].x = node->center.x - offset;
        centers[0].z = node->center.z - offset;
        centers[0].y = node->center.y;

        centers[1].x = node->center.x - offset;
        centers[1].z = node->center.z + offset;
        centers[1].y = node->center.y;

        centers[2].x = node->center.x + offset;
        centers[2].z = node->center.z - offset;
        centers[2].y = node->center.y;

        centers[3].x = node->center.x + offset;
        centers[3].z = node->center.z + offset;
        centers[3].y = node->center.y;

        nodes[ 0 ].initialize( terrain, centers[ 0 ], diameter, depth );
        nodes[ 1 ].initialize( terrain, centers[ 1 ], diameter, depth );
        nodes[ 2 ].initialize( terrain, centers[ 2 ], diameter, depth );
        nodes[ 3 ].initialize( terrain, centers[ 3 ], diameter, depth );
    }
    
    TerrainNodeChildren::~TerrainNodeChildren()
    {
        merge();
    }




    
    void FlatTerrainNode::initialize( FlatTerrain* terrain, const dvec3& center, uint32_t diameter, uint32_t depth )
    {
        this->children = nullptr;
        this->terrain = terrain;
        this->diameter = diameter;
        this->center = center;
        this->depth = depth;
    }

    void FlatTerrainNode::render( FlatTerrainRenderer& renderer )
    {
        if ( children )
        {
            children->render( renderer );
        }
    }

    bool FlatTerrainNode::splitable( const dvec3& eye )const
    {
        double dx = eye.x - center.x;
        double dy = eye.y - center.y;
        double dz = eye.z - center.z;
        double radius_sum = diameter + terrain->settings()->view_radius;
        double distance_squared = sqrt(dx * dx + dy * dy + dz * dz);
//        double radius_squared = radius_sum * radius_sum;
        return (distance_squared <= radius_sum);
    }

    void FlatTerrainNode::update( const dvec3& eye )
    {
        if( splitable( eye ) )
        {
            if ( !children && diameter >= terrain->settings()->terrain_diameter )
            {
                split();
            }

            if ( children )
            {
                children->update( eye );
            }
        }
        else if ( children )
        {
            merge();
        }
    }

    void FlatTerrainNode::split()
    {
        if ( diameter > terrain->settings()->terrain_diameter )
        {
            children = new TerrainNodeChildren( terrain, this );
        }
        else
        {
            FlatTerrainTile* tile = new FlatTerrainTile( this );
            terrain->generateHeightmapTile( tile );
            children = tile;
        }
    }

    void FlatTerrainNode::merge()
    {
        if ( children )
        {
            children->merge();
            children.clear();
            //delete children;
            //children = nullptr;
        }
    }
    
    FlatTerrainNode::FlatTerrainNode()
    :   terrain( nullptr )
    ,   children( nullptr )
    ,   center( 0,0,0 )
    ,   diameter( 0 )
    ,   depth( 0 )
    {}
}
