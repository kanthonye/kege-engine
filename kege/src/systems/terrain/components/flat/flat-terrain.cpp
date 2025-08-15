//
//  flat-landscape-quadtree.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/30/24.
//

#include "normal-map-generator.hpp"
#include "height-map-generator.hpp"
#include "task-manager-system.hpp"
#include "flat-terrain.hpp"
#include "flat-terrain-tile.hpp"

namespace kege{

    void FlatTerrain::render( kege::CommandBuffer& command_buffer )
    {
        _renderer.beginRender( command_buffer );
        _root.render( _renderer );
        _renderer.endRender( command_buffer );
    }

    void FlatTerrain::update( const dvec3& eye )
    {
        // get the newly generated terrain tiles if any
        std::vector< GeneratedTerrainTile > tiles;
        {
            std::lock_guard< std::mutex > lock( _mutex_generated_tiles );
            tiles.swap( _generated_tiles );
        }
        
        // initalize the newly generated terrain tiles if any
        for ( auto itr = tiles.begin(); itr != tiles.end(); ++itr )
        {
            itr->first->initialize( itr->second );
        }

        // then update all the terrain nodes
        _root.update( eye );
    }

    bool FlatTerrain::initialize( const kege::LandscapeSettings& settings )
    {
        if ( _init )
        {
            return true;
        }

        _settings = settings;

        // ensure the patch diameter is not less than 2.
        _settings.patch_diameter = kege::max<uint32_t>(_settings.patch_diameter, 2);
        // ensure the terrain diameter is not less than the patch diameter.
        _settings.terrain_diameter = kege::max<uint32_t>(_settings.terrain_diameter, _settings.patch_diameter);
        // ensure the landscape diameter is not less than the terrain diameter.
        _settings.landscape_diameter = kege::max<uint32_t>(_settings.landscape_diameter, _settings.terrain_diameter);

        _half_patch_parameter = _settings.patch_diameter * 0.5;
        _terrain_div_patch = 1 + 2 * ( _settings.terrain_diameter / _settings.patch_diameter );

        if ( !_renderer.initialize() )
        {
            return false;
        }

        bool init = _image_layer_manager.initialize
        (
            _settings.heightmap_diameter,
            _settings.heightmap_diameter,
            _settings.max_image_array_layers
        );

        if ( !init )
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
            new HeightmapGenerator( settings.heightmap_diameter, settings.terrain_diameter, layer_settings )
        });
        _topography_generator.addSurfaceGenerator({ new NormalmapGenerator( 32, _settings.heightmap_diameter ) });


        _root.initialize( this, settings.position, settings.landscape_diameter, 0 );
        _init = true;
        return true;
    }

    void FlatTerrain::generateTerrainTopography( Ref< FlatTerrainTile > tile )
    {
        double ts = settings()->terrain_diameter * 0.5;
        Ref< TerrainTopography > topography = _topography_generator.generate
        (
            tile->_root.center.x - ts, tile->_root.center.z - ts
        );
        std::lock_guard< std::mutex > lock( _mutex_generated_tiles );
        _generated_tiles.push_back({ tile, topography });
    }
    
    void FlatTerrain::generateHeightmapTile( Ref< FlatTerrainTile > tile )
    {
        TaskManagerSystem::addTask( [ this, tile ](){ this->generateTerrainTopography( tile ); } );
    }
    
    sint2 FlatTerrain::calcTileCoord( const dvec3& tile_position )
    {
        dvec2 coord;
        int diameter = settings()->landscape_diameter / settings()->terrain_diameter;
        coord.x = (tile_position.x - _root.center.x) + _root.diameter * 0.5;
        coord.y = (tile_position.z - _root.center.z) + _root.diameter * 0.5;
        coord = (coord / _root.diameter) * diameter;
        return sint2(coord.x, coord.y);
    }

    uint64_t FlatTerrain::calcTileIndex( const sint2& coord )
    {
        return (static_cast< uint64_t >( coord.x ) << 32) | coord.y;
    }

    void FlatTerrain::remove( const sint2& coord )
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

    void FlatTerrain::insert( const sint2& coord, FlatTerrainTile* tile )
    {
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
            tile->_root.setNeighborNorth( &north->_root );
            north->_root.setNeighborSouth( &tile->_root );
        }
        if ( south )
        {
            tile->_root.setNeighborSouth( &south->_root );
            south->_root.setNeighborNorth( &tile->_root );
        }
        if ( east )
        {
            tile->_root.setNeighborEast( &east->_root );
            east->_root.setNeighborWest( &tile->_root );
        }
        if ( west )
        {
            tile->_root.setNeighborWest( &west->_root );
            west->_root.setNeighborEast( &tile->_root );
        }
    }

    FlatTerrainTile* FlatTerrain::getTile( const sint2& coord )
    {
        auto m = _tiles.find( calcTileIndex( coord ) );
        if ( m != _tiles.end() )
        {
            return m->second;
        }
        return nullptr;
    }

    FlatTerrain::FlatTerrain()
    :   _renderer( this )
    {}

    FlatTerrain::~FlatTerrain()
    {
        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
        {
            delete (*itr);
        }
        _layers.clear();
    }


}






