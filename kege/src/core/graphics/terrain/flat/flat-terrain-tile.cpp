//
//  flat-terrain-tile.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/11/25.
//

#include "flat-terrain-node.hpp"
#include "flat-terrain-tile.hpp"
#include "flat-terrain-renderer.hpp"
#include "flat-terrain.hpp"

namespace kege{

    void FlatTerrainTile::render()
    {
        render( _root );
    }

    void FlatTerrainTile::update()
    {
        update( _root );
    }

//    void FlatTerrainTile::initialize( FlatTerrainQuadtree& node, const dvec3& center, uint32_t diameter, uint32_t depth )
//    {
//        node.children   = nullptr;
//        node.diameter   = diameter;
//        node.center     = center;
//        node.depth      = depth;
//
//        node.patch.x = center.x;
//        node.patch.y = center.z;
//        node.patch.radius = diameter * 0.5;
//        //node.patch.index_buffer_id = 0;
//
//        node.patch.image_index = _image_layer.imageIndex();
//        node.patch.image_layer = _image_layer.imageLayer();
//        node.patch.tile_position[0] = _root.center[0];
//        node.patch.tile_position[1] = _root.center[2];
//
//        node.patch.color = color;
//
//        node.local.x = (node.center.x - _root.center.x) + _root.patch.radius;
//        node.local.y = (node.center.z - _root.center.z) + _root.patch.radius;
//
//        setHeight( node );
//    }

    void FlatTerrainTile::render( FlatTerrainQuadtree& node )
    {
        if ( node.children )
        {
            render( node.children->nw );
            render( node.children->ne );
            render( node.children->sw );
            render( node.children->se );
        }
        else
        {
            _terrain->_renderer->submit( node.patch );
        }
    }

    bool FlatTerrainTile::splitable( FlatTerrainQuadtree& node )const
    {
        kege::dvec3 node_to_eye = _terrain->_camera_position - node.center;
        double dist = magnSq( node_to_eye );

        double length_sq = kege::sq< double >( node.diameter + node.diameter );
        double resolution = (dist / length_sq);
        return node.diameter > _terrain->settings().patch_diameter && (resolution < _terrain->settings().max_terrain_resolution);
        //return resolution < _landscape->settings()->resolutions[node.depth] && node.diameter > _landscape->settings()->patch_diameter;
    }

    void FlatTerrainTile::update( FlatTerrainQuadtree& node )
    {
        if( splitable( node ) )
        {
            if ( !node.children )
            {
                if ( node.depth < _terrain->settings().max_terrain_depth )
                {
                    node.split( this );
                }
            }

            if ( node.children )
            {
                update( node.children->nw );
                update( node.children->ne );
                update( node.children->sw );
                update( node.children->se );
            }
        }
        else if ( node.children )
        {
            node.merge();
        }
    }

//    void FlatTerrainTile::split( FlatTerrainQuadtree& node )
//    {
//        uint32_t child_width  = node.diameter * 0.5;
//        uint32_t child_offset = child_width * 0.5;
//        uint32_t child_depth  = node.depth  + 1;
//
//        dvec3 centers[4];
//
//        centers[0].x = node.center.x - child_offset;
//        centers[0].z = node.center.z + child_offset;
//        centers[0].y = node.center.y;
//
//        centers[1].x = node.center.x + child_offset;
//        centers[1].z = node.center.z + child_offset;
//        centers[1].y = node.center.y;
//
//        centers[2].x = node.center.x - child_offset;
//        centers[2].z = node.center.z - child_offset;
//        centers[2].y = node.center.y;
//
//        centers[3].x = node.center.x + child_offset;
//        centers[3].z = node.center.z - child_offset;
//        centers[3].y = node.center.y;
//
//        node.children = new FlatTerrainQuadtreeChildren;
//        initialize( node.children->nw, centers[0], child_width, child_depth );
//        initialize( node.children->ne, centers[1], child_width, child_depth );
//        initialize( node.children->sw, centers[2], child_width, child_depth );
//        initialize( node.children->se, centers[3], child_width, child_depth );
//
//        /*
//         A child descendent can only have neighbors if and only if that neighbor and
//         the child are at the same depth. This is a crucial step in correcting the
//         disuniform LOD between terrain nodes. If a terrain tile node's adjacent
//         neighbor is null, then the edge center vertex between the terrain tile node
//         and its adjacent null neighbor is disabled.
//         */
//
//        node.children->nw.setNeighborEast( &node.children->ne );
//        node.children->ne.setNeighborWest( &node.children->nw );
//        node.children->sw.setNeighborEast( &node.children->se );
//        node.children->se.setNeighborWest( &node.children->sw );
//
//        node.children->nw.setNeighborSouth( &node.children->sw );
//        node.children->sw.setNeighborNorth( &node.children->nw );
//        node.children->ne.setNeighborSouth( &node.children->se );
//        node.children->se.setNeighborNorth( &node.children->ne );
//
//        if ( node.neighbor.north )
//        {
//            if( node.neighbor.north->children )
//            {
//                node.neighbor.north->children->sw.setNeighborSouth( &node.children->nw );
//                node.children->nw.setNeighborNorth( &node.neighbor.north->children->sw );
//
//                node.neighbor.north->children->se.setNeighborSouth( &node.children->ne );
//                node.children->ne.setNeighborNorth( &node.neighbor.north->children->se );
//            }
//        }
//        if ( node.neighbor.south )
//        {
//            if( node.neighbor.south->children )
//            {
//                node.neighbor.south->children->nw.setNeighborNorth( &node.children->sw );
//                node.children->sw.setNeighborSouth( &node.neighbor.south->children->nw );
//
//                node.neighbor.south->children->ne.setNeighborNorth( &node.children->se );
//                node.children->se.setNeighborSouth( &node.neighbor.south->children->ne );
//            }
//        }
//        if ( node.neighbor.east )
//        {
//            if( node.neighbor.east->children )
//            {
//                node.neighbor.east->children->nw.setNeighborWest( &node.children->ne );
//                node.children->ne.setNeighborEast( &node.neighbor.east->children->nw );
//
//                node.neighbor.east->children->sw.setNeighborWest( &node.children->se );
//                node.children->se.setNeighborEast( &node.neighbor.east->children->sw );
//            }
//        }
//        if ( node.neighbor.west )
//        {
//            if( node.neighbor.west->children )
//            {
//                node.neighbor.west->children->ne.setNeighborEast( &node.children->nw );
//                node.children->nw.setNeighborWest( &node.neighbor.west->children->ne );
//
//                node.neighbor.west->children->se.setNeighborEast( &node.children->sw );
//                node.children->sw.setNeighborWest( &node.neighbor.west->children->se );
//            }
//        }
//    }

//    void FlatTerrainTile::merge( FlatTerrainQuadtree& node )
//    {
//        if ( node.children )
//        {
//            merge( node.children->nw );
//            merge( node.children->ne );
//            merge( node.children->sw );
//            merge( node.children->se );
//
//            delete node.children;
//            node.children = nullptr;
//        }
//    }

    void FlatTerrainTile::setHeight( FlatTerrainQuadtree& node )
    {
        float mx = (node.center.x - _root.center.x) / _root.diameter + 0.5;
        float my = (node.center.z - _root.center.z) / _root.diameter + 0.5;
        int hx = mx * _terrain->settings().heightmap_diameter;
        int hy = my * _terrain->settings().heightmap_diameter;
        int i =  hx + hy * _terrain->settings().heightmap_diameter;
        node.center.y = lerp< double >
        (
            _terrain->settings().min_height,
            _terrain->settings().max_height,
            _topography->heightmap->data[ i ]
        );
    }

    void FlatTerrainTile::initialize( Ref< TerrainTopography >& topography )
    {
        _topography.clear();
        _topography = topography;

        if ( _topography && !_image_layer )
        {
            const void* data[2] =
            {
                _topography->heightmap->data.data(),
                _topography->normalmap->data.data()
            };
            _image_layer = _terrain->_renderer->getImageLayerManager().generateImageLayer();
            _image_layer.setImageLayerData( data );
            _status = PENDING;
        }
        else
        {
            const void* data[2] =
            {
                _topography->heightmap->data.data(),
                _topography->normalmap->data.data()
            };
            _image_layer.setImageLayerData( data );
        }
    }

    void FlatTerrainTile::setNeighborNorth( FlatTerrainTile* node )
    {
//        neighbor.north = node;
//        if ( node == nullptr )
//        {
//            patch.index_buffer_id &= ~NORTH_VERTEX_BIT;
//        }
//        else
//        {
//            patch.index_buffer_id |= NORTH_VERTEX_BIT;
//        }
    }

    void FlatTerrainTile::setNeighborSouth( FlatTerrainTile* node )
    {
//        neighbor.south = node;
//        if ( node == nullptr )
//        {
//            patch.index_buffer_id &= ~SOUTH_VERTEX_BIT;
//        }
//        else
//        {
//            patch.index_buffer_id |= SOUTH_VERTEX_BIT;
//        }
    }

    void FlatTerrainTile::setNeighborEast( FlatTerrainTile* node )
    {
//        neighbor.east = node;
//        if ( node == nullptr )
//        {
//            patch.index_buffer_id &= ~EAST_VERTEX_BIT;
//        }
//        else
//        {
//            patch.index_buffer_id |= EAST_VERTEX_BIT;
//        }
    }

    void FlatTerrainTile::setNeighborWest( FlatTerrainTile* node )
    {
//        neighbor.west = node;
//        if ( node == nullptr )
//        {
//            patch.index_buffer_id &= ~WEST_VERTEX_BIT;
//        }
//        else
//        {
//            patch.index_buffer_id |= WEST_VERTEX_BIT;
//        }
    }

    const FlatTerrainQuadtree& FlatTerrainTile::getRoot()const
    {
        return _root;
    }

    const ImageLayer& FlatTerrainTile::getImageLayer()const
    {
        return _image_layer;
    }

    FlatTerrainTile::FlatTerrainTile( PhysicalFlatTerrain* terrain, const sint2& coord )
    :   _terrain( terrain )
    ,   _coord_max( coord + 1 )
    ,   _coord_min( coord - 1 )
    ,   _coord( coord )
    ,   _status( IDLE )
    {
        color = vec4( rand3f( rand1f(0.6,1), rand1f(0.6,1), rand1f(0.6,1) ).gen(), 1.0 );

        _root.children  = nullptr;
        _root.diameter  = _terrain->settings().terrain_tile_diameter;

        _root.center.y  = 0;
        _root.center.x  = coord.x * _terrain->settings().terrain_tile_diameter;
        _root.center.z  = coord.y * _terrain->settings().terrain_tile_diameter;
    }

    FlatTerrainTile::~FlatTerrainTile()
    {
        _root.merge();

        _topography.clear();
        _image_layer.clear();

        _terrain = nullptr;
        _status = IDLE;
    }

}
