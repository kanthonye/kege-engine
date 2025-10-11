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
        
        _mesh_source = new MeshSource;
        _mesh_source->primative = _terrain->_primative;
        _mesh_source->primitive_type = PrimitiveType::Mesh;

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

    FlatTerrainTile::FlatTerrainTile( FlatTerrain* terrain, const sint2& coord )
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
