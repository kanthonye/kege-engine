//
//  flat-landscape-quadtree.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/30/24.
//

#include "normal-map-generator.hpp"
#include "height-map-generator.hpp"
#include "task-manager-system.hpp"
#include "flat-terrain-tile.hpp"
#include "flat-terrain.hpp"

namespace kege{

    void PhysicalFlatTerrain::render( kege::CommandEncoder* encoder, Transform* transform )
    {
        _renderer->begin( encoder, transform );
        for ( auto itr = _tiles.begin(); itr != _tiles.end(); ++itr )
        {
            itr->second->render();
        }
        _renderer->end();
    }

    void PhysicalFlatTerrain::update( const kege::dvec3& camera_position )
    {
        _camera_position = camera_position;

        sint2 center;
        center.x = camera_position.x / _settings.terrain_tile_diameter;
        center.y = camera_position.z / _settings.terrain_tile_diameter;

        int padding = max<int>( 1, _settings.view_radius / _settings.terrain_tile_diameter);

        sint2 min,max;
        min.x = center.x - padding;
        min.y = center.y - padding;
        max.x = center.x + padding;
        max.y = center.y + padding;

        // remove out of range tiles
        std::vector< std::map< uint64_t, FlatTerrainTile* >::iterator > iterators;
        for ( auto itr = _tiles.begin(); itr != _tiles.end(); ++itr )
        {
            FlatTerrainTile* tile = itr->second;
            if
            (
             max.x < tile->_coord_min.x || min.x > tile->_coord_max.x ||
             max.y < tile->_coord_min.y || min.y > tile->_coord_max.y
            )
            {
                iterators.push_back( itr );
            }
        }
        for ( int i = 0; i < iterators.size(); ++i )
        {
            _tiles.erase( iterators[i] );
        }

        for (int y=min.x; y<max.x; ++y)
        {
            for (int x=min.x; x<max.x; ++x)
            {
                sint2 coord = {x, y};
                uint64_t key = calcTileIndex( coord );
                auto m = _tiles.find( key );
                if ( m == _tiles.end() )
                {
                    insert( coord );
                }
                else
                {
                    m->second->update();
                }
            }
        }



//        // initalize the newly generated terrain tiles if any
//        for ( auto itr = tiles.begin(); itr != tiles.end(); ++itr )
//        {
//            itr->first->initialize( itr->second );
//        }
//
//        // then update all the terrain nodes
//        _root.update( camera_position );
    }

    bool PhysicalFlatTerrain::initialize()
    {
        if ( _renderer )
        {
            return true;
        }

        // ensure the patch diameter is not less than 2.
        _settings.patch_diameter = kege::max<uint32_t>(_settings.patch_diameter, 2);
        // ensure the terrain diameter is not less than the patch diameter.
        _settings.terrain_diameter = kege::max<uint32_t>(_settings.terrain_diameter, _settings.patch_diameter);
        // ensure the landscape diameter is not less than the terrain diameter.
        _settings.landscape_diameter = kege::max<uint32_t>(_settings.landscape_diameter, _settings.terrain_diameter);

        _half_patch_parameter = _settings.patch_diameter * 0.5;
        _terrain_div_patch = 1 + 2 * ( _settings.terrain_diameter / _settings.patch_diameter );

        _renderer = new FlatTerrainRenderer( _graphics );
        if ( !_renderer->initialize( &_settings ) )
        {
            return false;
        }

        std::vector< HeightmapLayerSetting > layer_settings(1);
        layer_settings[0].permutation = new PermutationTable3D( getPermutationTable3D() );
        layer_settings[0].noiseFunct = HeightmapGenerator::fractalNoise;
        layer_settings[0].heightmap.offset = {10000, 80, 0};
        layer_settings[0].heightmap.persistance = 0.75;
        layer_settings[0].heightmap.lacunarity = 2;
        layer_settings[0].heightmap.steepness = 1.5;
        layer_settings[0].heightmap.octaves = 6;
        layer_settings[0].heightmap.scale = 4000;

        _topography_generator.addSurfaceGenerator
        ({
            new HeightmapGenerator( _settings.heightmap_diameter, _settings.terrain_diameter, layer_settings )
        });
        _topography_generator.addSurfaceGenerator({ new NormalmapGenerator( 32, _settings.heightmap_diameter ) });


        //_root.initialize( this, _settings.position, _settings.landscape_diameter, 0 );
        //_init = true;
        return true;
    }

    void PhysicalFlatTerrain::generateTerrainTopography( Ref< FlatTerrainTile > tile )
    {
        double ts = _settings.terrain_diameter * 0.5;
        Ref< TerrainTopography > topography = _topography_generator.generate
        (
            tile->_root.center.x - ts, tile->_root.center.z - ts
        );
        std::lock_guard< std::mutex > lock( _mutex_generated_tiles );
        _generated_tiles.push_back({ tile, topography });
    }
    
    void PhysicalFlatTerrain::generateHeightmapTile( Ref< FlatTerrainTile > tile )
    {
        TaskManagerSystem::addTask( [ this, tile ](){ this->generateTerrainTopography( tile ); } );
    }
    
    sint2 PhysicalFlatTerrain::calcTileCoord( const dvec3& tile_position )
    {
        dvec2 coord;
        int diameter = _settings.landscape_diameter / _settings.terrain_diameter;
//        coord.x = (tile_position.x - _root.center.x) + _root.diameter * 0.5;
//        coord.y = (tile_position.z - _root.center.z) + _root.diameter * 0.5;
//        coord = (coord / _root.diameter) * diameter;
        return sint2(coord.x, coord.y);
    }

    uint64_t PhysicalFlatTerrain::calcTileIndex( const sint2& coord )
    {
        return (static_cast< uint64_t >( coord.x ) << 32) | coord.y;
    }

    void PhysicalFlatTerrain::remove( const sint2& coord )
    {
        auto m = _tiles.find( calcTileIndex( coord ) );
        if ( m != _tiles.end() )
        {
            FlatTerrainTile* tile = m->second;
            if ( tile->_root.neighbor.north )
            {
                tile->_root.neighbor.north->neighbor.south = nullptr;
            }
            if ( tile->_root.neighbor.south )
            {
                tile->_root.neighbor.south->neighbor.north = nullptr;
            }
            if ( tile->_root.neighbor.east )
            {
                tile->_root.neighbor.east->neighbor.west = nullptr;
            }
            if ( tile->_root.neighbor.west )
            {
                tile->_root.neighbor.west->neighbor.east = nullptr;
            }
            _tiles.erase( m );
        }
    }

    void PhysicalFlatTerrain::insert( const sint2& coord )
    {
        FlatTerrainTile* tile = new FlatTerrainTile( this, coord );
        uint64_t key = calcTileIndex( coord );
        _tiles[ key ] = tile;

        // convert the tile coordinate to and integer index. Store the terrain into the map using the index as key
        uint64_t index = calcTileIndex( coord );
        _tiles[ index ] = tile;

        // assign the terrain tile neighbors
        FlatTerrainTile* north = getTile( coord + sint2(0, 1) );
        FlatTerrainTile* south = getTile( coord - sint2(0, 1) );
        FlatTerrainTile* east  = getTile( coord + sint2(1, 0) );
        FlatTerrainTile* west  = getTile( coord - sint2(1, 0) );

        // update the neighbors of the terrain tile
        if ( north )
        {
            tile->setNeighborNorth( north );
            north->setNeighborSouth( tile );
        }
        if ( south )
        {
            tile->setNeighborSouth( south );
            south->setNeighborNorth( tile );
        }
        if ( east )
        {
            tile->setNeighborEast( east );
            east->setNeighborWest( tile );
        }
        if ( west )
        {
            tile->setNeighborWest( west );
            west->setNeighborEast( tile );
        }
    }

    FlatTerrainTile* PhysicalFlatTerrain::getTile( const sint2& coord )
    {
        auto m = _tiles.find( calcTileIndex( coord ) );
        if ( m != _tiles.end() )
        {
            return m->second;
        }
        return nullptr;
    }

    PhysicalFlatTerrain::PhysicalFlatTerrain( const kege::TerrainSettings& settings )
    :   PhysicalTerrain( settings )
    {}

    PhysicalFlatTerrain::~PhysicalFlatTerrain()
    {
//        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
//        {
//            delete (*itr);
//        }
//        _layers.clear();
    }


}






