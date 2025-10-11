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

    void FlatTerrain::render( kege::CommandEncoder* encoder, Transform* transform )
    {
        _renderer->begin( encoder, transform );
        for ( auto itr = _tiles.begin(); itr != _tiles.end(); ++itr )
        {
            itr->second->render();
        }
        _renderer->end();
    }

    void FlatTerrain::update( const kege::dvec3& camera_position )
    {
        _camera_position = camera_position;

        sint2 center;
        center.x = camera_position.x / _settings.terrain_tile_diameter;
        center.y = camera_position.z / _settings.terrain_tile_diameter;

        int padding = max< int >( 1, _settings.view_radius / _settings.terrain_tile_diameter );

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
    }

    bool FlatTerrain::initialize()
    {
        if ( _renderer )
        {
            return true;
        }

        createMeshPrimitive( _graphics, _primative, _draw_commands );

        // ensure the patch diameter is not less than 2.
        _settings.patch_diameter = kege::max<uint32_t>(_settings.patch_diameter, 2);
        // ensure the terrain diameter is not less than the patch diameter.
        _settings.terrain_diameter = kege::max<uint32_t>(_settings.terrain_diameter, _settings.patch_diameter);
        // ensure the landscape diameter is not less than the terrain diameter.
        _settings.landscape_diameter = kege::max<uint32_t>(_settings.landscape_diameter, _settings.terrain_diameter);

        _half_patch_parameter = _settings.patch_diameter * 0.5;
        _terrain_div_patch = 1 + 2 * ( _settings.terrain_diameter / _settings.patch_diameter );

//        _renderer = new FlatTerrainRenderer( _graphics );
//        if ( !_renderer->initialize( &_settings ) )
//        {
//            return false;
//        }

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

    void FlatTerrain::generateTerrainTopography( Ref< FlatTerrainTile > tile )
    {
        double ts = _settings.terrain_diameter * 0.5;
        Ref< TerrainTopography > topography = _topography_generator.generate
        (
            tile->_root.center.x - ts, tile->_root.center.z - ts
        );
        std::lock_guard< std::mutex > lock( _mutex_generated_tiles );
        _generated_tiles.push_back({ tile, topography });
    }
    
    void FlatTerrain::generateHeightmapTile( Ref< FlatTerrainTile > tile )
    {
        QueueManagerSystem::addTask( [ this, tile ](){ this->generateTerrainTopography( tile ); } );
    }
    
    sint2 FlatTerrain::calcTileCoord( const dvec3& tile_position )
    {
        dvec2 coord;
//        int diameter = _settings.landscape_diameter / _settings.terrain_diameter;
//        coord.x = (tile_position.x - _root.center.x) + _root.diameter * 0.5;
//        coord.y = (tile_position.z - _root.center.z) + _root.diameter * 0.5;
//        coord = (coord / _root.diameter) * diameter;
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

    void FlatTerrain::insert( const sint2& coord )
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

    FlatTerrainTile* FlatTerrain::getTile( const sint2& coord )
    {
        auto m = _tiles.find( calcTileIndex( coord ) );
        if ( m != _tiles.end() )
        {
            return m->second;
        }
        return nullptr;
    }

    FlatTerrain::FlatTerrain( const kege::TerrainSettings& settings )
    :   PhysicalTerrain( nullptr )
    {}

    FlatTerrain::~FlatTerrain()
    {
//        for ( Layers::iterator itr = _layers.begin(); itr != _layers.end(); ++itr )
//        {
//            delete (*itr);
//        }
//        _layers.clear();
    }





    void createMeshPrimitive( Graphics* graphics, kege::Ref< MeshPrimitive >& primative, std::vector< IndexDrawCommand >& draw_commands )
    {
        uint1 index_array_size = 24;
        uint1 index_array_count = 16;
        uint1 index_buffer_size = index_array_size * index_array_count;

        draw_commands.resize( index_array_count );
        std::vector< uint32_t > indices( index_buffer_size );
        memset( indices.data(), 0, indices.size() * sizeof( indices[0] ) );
        {
            uint16_t i = 0, j = 0; // 0000 = 0
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 12;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  8; // 3    4    5
            indices[  6 + i ] =  8; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  6; // | /     \ |
            indices[  9 + i ] =  6; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  0;

            j = 1; // 0001 = 1
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 15;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3    4    5
            indices[  6 + i ] =  2; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  8; // | /     \ |
            indices[  9 + i ] =  8; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  0;

            j = 2; // 0010 = 2
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 15;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  5; // 3    4----5
            indices[  6 + i ] =  5; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  8; // | /     \ |
            indices[  9 + i ] =  8; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  0;

            j = 3; // 0011 = 3
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3    4----5
            indices[  6 + i ] =  2; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  4; // | /     \ |
            indices[  9 + i ] =  5; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  8;
            indices[ 12 + i ] =  8;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;

            j = 4; // 0100 = 4
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 15;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  8; // 3    4    5
            indices[  6 + i ] =  8; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  7; // | /  |  \ |
            indices[  9 + i ] =  7; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  0;

            j = 5; // 0101 = 5
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3    4    5
            indices[  6 + i ] =  2; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  8; // | /  |  \ |
            indices[  9 + i ] =  8; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  7;
            indices[ 12 + i ] =  7;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;

            j = 6; // 0110 = 6
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  5; // 3    4----5
            indices[  6 + i ] =  5; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  8; // | /  |  \ |
            indices[  9 + i ] =  8; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  7;
            indices[ 12 + i ] =  7;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;

            j = 7; // 0111 = 7
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 21;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3    4----5
            indices[  6 + i ] =  2; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  5; // | /  |  \ |
            indices[  9 + i ] =  5; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  8;
            indices[ 12 + i ] =  8;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  7;
            indices[ 15 + i ] =  7;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  6;
            indices[ 18 + i ] =  6;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  0;

            j = 8; // 1000 = 8
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 15;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  8; // 3----4    5
            indices[  6 + i ] =  8; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  6; // | /     \ |
            indices[  9 + i ] =  6; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  3;
            indices[ 12 + i ] =  3;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  0;
            indices[ 15 + i ] =  0;
            indices[ 16 + i ] =  0;
            indices[ 17 + i ] =  0;
            indices[ 18 + i ] =  0;
            indices[ 19 + i ] =  0;
            indices[ 20 + i ] =  0;

            j = 9; // 1001 = 9
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3----4    5
            indices[  6 + i ] =  2; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  8; // | /     \ |
            indices[  9 + i ] =  8; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  3;
            indices[ 15 + i ] =  3;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;
            indices[ 18 + i ] =  0;
            indices[ 19 + i ] =  0;
            indices[ 20 + i ] =  0;

            j = 10; // 1010 = 10
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  5; // 3----4----5
            indices[  6 + i ] =  5; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  8; // | /     \ |
            indices[  9 + i ] =  8; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  3;
            indices[ 15 + i ] =  3;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;
            indices[ 18 + i ] =  0;
            indices[ 19 + i ] =  0;
            indices[ 20 + i ] =  0;

            j = 11; // 1011 = 11
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 21;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3----4----5
            indices[  6 + i ] =  2; // |   / \   |
            indices[  7 + i ] =  4; // |  /   \  |
            indices[  8 + i ] =  5; // | /     \ |
            indices[  9 + i ] =  5; // |/       \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  8;
            indices[ 12 + i ] =  8;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  3;
            indices[ 18 + i ] =  3;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  0;

            j = 12; // 1100 = 12
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 18;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  8; // 3----4    5
            indices[  6 + i ] =  8; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  7; // | /  |  \ |
            indices[  9 + i ] =  7; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  6;
            indices[ 12 + i ] =  6;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  3;
            indices[ 15 + i ] =  3;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  0;
            indices[ 18 + i ] =  0;
            indices[ 19 + i ] =  0;
            indices[ 20 + i ] =  0;

            j = 13; // 1101 = 13
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 21;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3----4    5
            indices[  6 + i ] =  2; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  8; // | /  |  \ |
            indices[  9 + i ] =  8; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  7;
            indices[ 12 + i ] =  7;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  3;
            indices[ 18 + i ] =  3;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  0;

            j = 14; // 1110 = 14
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 21;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\       /|
            indices[  2 + i ] =  2; // | \     / |
            indices[  3 + i ] =  2; // |  \   /  |
            indices[  4 + i ] =  4; // |   \ /   |
            indices[  5 + i ] =  5; // 3----4----5
            indices[  6 + i ] =  5; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  8; // | /  |  \ |
            indices[  9 + i ] =  8; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  7;
            indices[ 12 + i ] =  7;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  6;
            indices[ 15 + i ] =  6;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  3;
            indices[ 18 + i ] =  3;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  0;

            j = 15; // 1111 = 15
            i = j * index_array_size;
            draw_commands[ j ].vertex_offset  = 0;
            draw_commands[ j ].first_instance = 0;
            draw_commands[ j ].instance_count = 1;
            draw_commands[ j ].first_index    = i;
            draw_commands[ j ].index_count    = 24;
            indices[  0 + i ] =  0; // 0----1----2
            indices[  1 + i ] =  4; // |\   |   /|
            indices[  2 + i ] =  1; // | \  |  / |
            indices[  3 + i ] =  1; // |  \ | /  |
            indices[  4 + i ] =  4; // |   \|/   |
            indices[  5 + i ] =  2; // 3----4----5
            indices[  6 + i ] =  2; // |   /|\   |
            indices[  7 + i ] =  4; // |  / | \  |
            indices[  8 + i ] =  5; // | /  |  \ |
            indices[  9 + i ] =  5; // |/   |   \|
            indices[ 10 + i ] =  4; // 6----7----8
            indices[ 11 + i ] =  8;
            indices[ 12 + i ] =  8;
            indices[ 13 + i ] =  4;
            indices[ 14 + i ] =  7;
            indices[ 15 + i ] =  7;
            indices[ 16 + i ] =  4;
            indices[ 17 + i ] =  6;
            indices[ 18 + i ] =  6;
            indices[ 19 + i ] =  4;
            indices[ 20 + i ] =  3;
            indices[ 21 + i ] =  3;
            indices[ 22 + i ] =  4;
            indices[ 23 + i ] =  0;
        }

        size_t indices_arraysize = indices.size() * sizeof( indices[0] );

        primative->index_buffer = graphics->createBuffer
        ({
            .size = indices_arraysize,
            .data = indices.data(),
            .usage = BufferUsage::IndexBuffer,
            .memory_usage = MemoryUsage::GpuOnly,
            .name = "IndexBuffers"
        });

        const uint1 vertex_count = 9;
        const vec4 vertex[ vertex_count ]
        {
            vec4( -1.f,-1.f   , 0.f, 0.f ),
            vec4(  0.f,-1.f   , 0.f, 0.f ),
            vec4(  1.f,-1.f   , 0.f, 0.f ),
            vec4( -1.f, 0.f   , 0.f, 0.f ),
            vec4(  0.f, 0.f   , 0.f, 0.f ),
            vec4(  1.f, 0.f   , 0.f, 0.f ),
            vec4( -1.f, 1.f   , 0.f, 0.f ),
            vec4(  0.f, 1.f   , 0.f, 0.f ),
            vec4(  1.f, 1.f   , 0.f, 0.f )
        };
        primative->vertex_buffer = graphics->createBuffer
        ({
            .size = vertex_count * sizeof( vertex[0] ),
            .data = vertex,
            .usage = BufferUsage::VertexBuffer,
            .memory_usage = MemoryUsage::GpuOnly,
            .name = "IndexBuffers"
        });
    }

}






