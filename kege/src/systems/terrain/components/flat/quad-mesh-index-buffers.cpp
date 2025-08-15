//
//  flat-landscape-patch.cpp
//  kege
//
//  Created by Kenneth Esdaile on 11/27/24.
//

#include "quad-mesh-index-buffers.hpp"
namespace kege{

    uint16_t QuadMeshIndexBuffers::getIndexBufferDrawCount(int i)
    {
        return _index_array_drawcounts[i];
    }
    ResourceSet& QuadMeshIndexBuffers::getGeometryBuffer()
    {
        return _resource_set;
    }

    void QuadMeshIndexBuffers::destroy()
    {
        _index_uniform_buffer.clear();
        if ( _resource_set ) kege::Graphics::freeResourceSet( _resource_set );
    }

    bool QuadMeshIndexBuffers::initialize()
    {
        if ( _resource_set )
        {
            return true;
        }

        uint1 index_array_size = 24;
        uint1 index_array_count = 16;

        _index_arrays.resize( index_array_count );
        _index_array_drawcounts.resize( index_array_count );
        memset( _index_arrays.data(), 0, _index_arrays.size() * sizeof( _index_arrays[0] ) );
        {
            uint16_t i = 0; // 0000 = 0
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\       /|
            _index_arrays[ i ].indices[  2 ] =  2; // | \     / |
            _index_arrays[ i ].indices[  3 ] =  2; // |  \   /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \ /   |
            _index_arrays[ i ].indices[  5 ] =  8; // 3    4    5
            _index_arrays[ i ].indices[  6 ] =  8; // |   / \   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  /   \  |
            _index_arrays[ i ].indices[  8 ] =  6; // | /     \ |
            _index_arrays[ i ].indices[  9 ] =  6; // |/       \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  0;
            _index_array_drawcounts[ i ] = 12;

            i = 1; // 0001 = 1
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\   |   /|
            _index_arrays[ i ].indices[  2 ] =  1; // | \  |  / |
            _index_arrays[ i ].indices[  3 ] =  1; // |  \ | /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \|/   |
            _index_arrays[ i ].indices[  5 ] =  2; // 3    4    5
            _index_arrays[ i ].indices[  6 ] =  2; // |   / \   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  /   \  |
            _index_arrays[ i ].indices[  8 ] =  8; // | /     \ |
            _index_arrays[ i ].indices[  9 ] =  8; // |/       \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  6;
            _index_arrays[ i ].indices[ 12 ] =  6;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  0;
            _index_array_drawcounts[ i ] = 15;

            i = 2; // 0010 = 2
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\       /|
            _index_arrays[ i ].indices[  2 ] =  2; // | \     / |
            _index_arrays[ i ].indices[  3 ] =  2; // |  \   /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \ /   |
            _index_arrays[ i ].indices[  5 ] =  5; // 3    4----5
            _index_arrays[ i ].indices[  6 ] =  5; // |   / \   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  /   \  |
            _index_arrays[ i ].indices[  8 ] =  8; // | /     \ |
            _index_arrays[ i ].indices[  9 ] =  8; // |/       \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  6;
            _index_arrays[ i ].indices[ 12 ] =  6;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  0;
            _index_array_drawcounts[ i ] = 15;

            i = 3; // 0011 = 3
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\   |   /|
            _index_arrays[ i ].indices[  2 ] =  1; // | \  |  / |
            _index_arrays[ i ].indices[  3 ] =  1; // |  \ | /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \|/   |
            _index_arrays[ i ].indices[  5 ] =  2; // 3    4----5
            _index_arrays[ i ].indices[  6 ] =  2; // |   / \   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  /   \  |
            _index_arrays[ i ].indices[  8 ] =  4; // | /     \ |
            _index_arrays[ i ].indices[  9 ] =  5; // |/       \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  8;
            _index_arrays[ i ].indices[ 12 ] =  8;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  6;
            _index_arrays[ i ].indices[ 15 ] =  6;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  0;
            _index_array_drawcounts[ i ] = 18;

            i = 4; // 0100 = 4
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\       /|
            _index_arrays[ i ].indices[  2 ] =  2; // | \     / |
            _index_arrays[ i ].indices[  3 ] =  2; // |  \   /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \ /   |
            _index_arrays[ i ].indices[  5 ] =  8; // 3    4    5
            _index_arrays[ i ].indices[  6 ] =  8; // |   /|\   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  / | \  |
            _index_arrays[ i ].indices[  8 ] =  7; // | /  |  \ |
            _index_arrays[ i ].indices[  9 ] =  7; // |/   |   \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  6;
            _index_arrays[ i ].indices[ 12 ] =  6;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  0;
            _index_array_drawcounts[ i ] = 15;

            i = 5; // 0101 = 5
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\   |   /|
            _index_arrays[ i ].indices[  2 ] =  1; // | \  |  / |
            _index_arrays[ i ].indices[  3 ] =  1; // |  \ | /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \|/   |
            _index_arrays[ i ].indices[  5 ] =  2; // 3    4    5
            _index_arrays[ i ].indices[  6 ] =  2; // |   /|\   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  / | \  |
            _index_arrays[ i ].indices[  8 ] =  8; // | /  |  \ |
            _index_arrays[ i ].indices[  9 ] =  8; // |/   |   \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  7;
            _index_arrays[ i ].indices[ 12 ] =  7;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  6;
            _index_arrays[ i ].indices[ 15 ] =  6;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  0;
            _index_array_drawcounts[ i ] = 18;

            i = 6; // 0110 = 6
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\       /|
            _index_arrays[ i ].indices[  2 ] =  2; // | \     / |
            _index_arrays[ i ].indices[  3 ] =  2; // |  \   /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \ /   |
            _index_arrays[ i ].indices[  5 ] =  5; // 3    4----5
            _index_arrays[ i ].indices[  6 ] =  5; // |   /|\   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  / | \  |
            _index_arrays[ i ].indices[  8 ] =  8; // | /  |  \ |
            _index_arrays[ i ].indices[  9 ] =  8; // |/   |   \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  7;
            _index_arrays[ i ].indices[ 12 ] =  7;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  6;
            _index_arrays[ i ].indices[ 15 ] =  6;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  0;
            _index_array_drawcounts[ i ] = 18;

            i = 7; // 0111 = 7
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\   |   /|
            _index_arrays[ i ].indices[  2 ] =  1; // | \  |  / |
            _index_arrays[ i ].indices[  3 ] =  1; // |  \ | /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \|/   |
            _index_arrays[ i ].indices[  5 ] =  2; // 3    4----5
            _index_arrays[ i ].indices[  6 ] =  2; // |   /|\   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  / | \  |
            _index_arrays[ i ].indices[  8 ] =  5; // | /  |  \ |
            _index_arrays[ i ].indices[  9 ] =  5; // |/   |   \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  8;
            _index_arrays[ i ].indices[ 12 ] =  8;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  7;
            _index_arrays[ i ].indices[ 15 ] =  7;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  6;
            _index_arrays[ i ].indices[ 18 ] =  6;
            _index_arrays[ i ].indices[ 19 ] =  4;
            _index_arrays[ i ].indices[ 20 ] =  0;
            _index_array_drawcounts[ i ] = 21;

            i = 8; // 1000 = 8
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\       /|
            _index_arrays[ i ].indices[  2 ] =  2; // | \     / |
            _index_arrays[ i ].indices[  3 ] =  2; // |  \   /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \ /   |
            _index_arrays[ i ].indices[  5 ] =  8; // 3----4    5
            _index_arrays[ i ].indices[  6 ] =  8; // |   / \   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  /   \  |
            _index_arrays[ i ].indices[  8 ] =  6; // | /     \ |
            _index_arrays[ i ].indices[  9 ] =  6; // |/       \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  3;
            _index_arrays[ i ].indices[ 12 ] =  3;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  0;
            _index_arrays[ i ].indices[ 15 ] =  0;
            _index_arrays[ i ].indices[ 16 ] =  0;
            _index_arrays[ i ].indices[ 17 ] =  0;
            _index_arrays[ i ].indices[ 18 ] =  0;
            _index_arrays[ i ].indices[ 19 ] =  0;
            _index_arrays[ i ].indices[ 20 ] =  0;
            _index_array_drawcounts[ i ] = 15;

            i = 9; // 1001 = 9
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\   |   /|
            _index_arrays[ i ].indices[  2 ] =  1; // | \  |  / |
            _index_arrays[ i ].indices[  3 ] =  1; // |  \ | /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \|/   |
            _index_arrays[ i ].indices[  5 ] =  2; // 3----4    5
            _index_arrays[ i ].indices[  6 ] =  2; // |   / \   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  /   \  |
            _index_arrays[ i ].indices[  8 ] =  8; // | /     \ |
            _index_arrays[ i ].indices[  9 ] =  8; // |/       \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  6;
            _index_arrays[ i ].indices[ 12 ] =  6;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  3;
            _index_arrays[ i ].indices[ 15 ] =  3;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  0;
            _index_arrays[ i ].indices[ 18 ] =  0;
            _index_arrays[ i ].indices[ 19 ] =  0;
            _index_arrays[ i ].indices[ 20 ] =  0;
            _index_array_drawcounts[ i ] = 18;

            i = 10; // 1010 = 10
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\       /|
            _index_arrays[ i ].indices[  2 ] =  2; // | \     / |
            _index_arrays[ i ].indices[  3 ] =  2; // |  \   /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \ /   |
            _index_arrays[ i ].indices[  5 ] =  5; // 3----4----5
            _index_arrays[ i ].indices[  6 ] =  5; // |   / \   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  /   \  |
            _index_arrays[ i ].indices[  8 ] =  8; // | /     \ |
            _index_arrays[ i ].indices[  9 ] =  8; // |/       \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  6;
            _index_arrays[ i ].indices[ 12 ] =  6;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  3;
            _index_arrays[ i ].indices[ 15 ] =  3;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  0;
            _index_arrays[ i ].indices[ 18 ] =  0;
            _index_arrays[ i ].indices[ 19 ] =  0;
            _index_arrays[ i ].indices[ 20 ] =  0;
            _index_array_drawcounts[ i ] = 18;

            i = 11; // 1011 = 11
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\   |   /|
            _index_arrays[ i ].indices[  2 ] =  1; // | \  |  / |
            _index_arrays[ i ].indices[  3 ] =  1; // |  \ | /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \|/   |
            _index_arrays[ i ].indices[  5 ] =  2; // 3----4----5
            _index_arrays[ i ].indices[  6 ] =  2; // |   / \   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  /   \  |
            _index_arrays[ i ].indices[  8 ] =  5; // | /     \ |
            _index_arrays[ i ].indices[  9 ] =  5; // |/       \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  8;
            _index_arrays[ i ].indices[ 12 ] =  8;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  6;
            _index_arrays[ i ].indices[ 15 ] =  6;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  3;
            _index_arrays[ i ].indices[ 18 ] =  3;
            _index_arrays[ i ].indices[ 19 ] =  4;
            _index_arrays[ i ].indices[ 20 ] =  0;
            _index_array_drawcounts[ i ] = 21;

            i = 12; // 1100 = 12
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\       /|
            _index_arrays[ i ].indices[  2 ] =  2; // | \     / |
            _index_arrays[ i ].indices[  3 ] =  2; // |  \   /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \ /   |
            _index_arrays[ i ].indices[  5 ] =  8; // 3----4    5
            _index_arrays[ i ].indices[  6 ] =  8; // |   /|\   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  / | \  |
            _index_arrays[ i ].indices[  8 ] =  7; // | /  |  \ |
            _index_arrays[ i ].indices[  9 ] =  7; // |/   |   \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  6;
            _index_arrays[ i ].indices[ 12 ] =  6;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  3;
            _index_arrays[ i ].indices[ 15 ] =  3;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  0;
            _index_arrays[ i ].indices[ 18 ] =  0;
            _index_arrays[ i ].indices[ 19 ] =  0;
            _index_arrays[ i ].indices[ 20 ] =  0;
            _index_array_drawcounts[ i ] = 18;

            i = 13; // 1101 = 13
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\   |   /|
            _index_arrays[ i ].indices[  2 ] =  1; // | \  |  / |
            _index_arrays[ i ].indices[  3 ] =  1; // |  \ | /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \|/   |
            _index_arrays[ i ].indices[  5 ] =  2; // 3----4    5
            _index_arrays[ i ].indices[  6 ] =  2; // |   /|\   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  / | \  |
            _index_arrays[ i ].indices[  8 ] =  8; // | /  |  \ |
            _index_arrays[ i ].indices[  9 ] =  8; // |/   |   \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  7;
            _index_arrays[ i ].indices[ 12 ] =  7;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  6;
            _index_arrays[ i ].indices[ 15 ] =  6;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  3;
            _index_arrays[ i ].indices[ 18 ] =  3;
            _index_arrays[ i ].indices[ 19 ] =  4;
            _index_arrays[ i ].indices[ 20 ] =  0;
            _index_array_drawcounts[ i ] = 21;

            i = 14; // 1110 = 14
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\       /|
            _index_arrays[ i ].indices[  2 ] =  2; // | \     / |
            _index_arrays[ i ].indices[  3 ] =  2; // |  \   /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \ /   |
            _index_arrays[ i ].indices[  5 ] =  5; // 3----4----5
            _index_arrays[ i ].indices[  6 ] =  5; // |   /|\   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  / | \  |
            _index_arrays[ i ].indices[  8 ] =  8; // | /  |  \ |
            _index_arrays[ i ].indices[  9 ] =  8; // |/   |   \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  7;
            _index_arrays[ i ].indices[ 12 ] =  7;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  6;
            _index_arrays[ i ].indices[ 15 ] =  6;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  3;
            _index_arrays[ i ].indices[ 18 ] =  3;
            _index_arrays[ i ].indices[ 19 ] =  4;
            _index_arrays[ i ].indices[ 20 ] =  0;
            _index_array_drawcounts[ i ] = 21;

            i = 15; // 1111 = 15
            _index_arrays[ i ].indices[  0 ] =  0; // 0----1----2
            _index_arrays[ i ].indices[  1 ] =  4; // |\   |   /|
            _index_arrays[ i ].indices[  2 ] =  1; // | \  |  / |
            _index_arrays[ i ].indices[  3 ] =  1; // |  \ | /  |
            _index_arrays[ i ].indices[  4 ] =  4; // |   \|/   |
            _index_arrays[ i ].indices[  5 ] =  2; // 3----4----5
            _index_arrays[ i ].indices[  6 ] =  2; // |   /|\   |
            _index_arrays[ i ].indices[  7 ] =  4; // |  / | \  |
            _index_arrays[ i ].indices[  8 ] =  5; // | /  |  \ |
            _index_arrays[ i ].indices[  9 ] =  5; // |/   |   \|
            _index_arrays[ i ].indices[ 10 ] =  4; // 6----7----8
            _index_arrays[ i ].indices[ 11 ] =  8;
            _index_arrays[ i ].indices[ 12 ] =  8;
            _index_arrays[ i ].indices[ 13 ] =  4;
            _index_arrays[ i ].indices[ 14 ] =  7;
            _index_arrays[ i ].indices[ 15 ] =  7;
            _index_arrays[ i ].indices[ 16 ] =  4;
            _index_arrays[ i ].indices[ 17 ] =  6;
            _index_arrays[ i ].indices[ 18 ] =  6;
            _index_arrays[ i ].indices[ 19 ] =  4;
            _index_arrays[ i ].indices[ 20 ] =  3;
            _index_arrays[ i ].indices[ 21 ] =  3;
            _index_arrays[ i ].indices[ 22 ] =  4;
            _index_arrays[ i ].indices[ 23 ] =  0;
            _index_array_drawcounts[ i ] = 24;
        }

        uint1 count = index_array_count * index_array_size;
        std::vector< uint4 > indices( count );
        for (int j = 0, i = 0; i < index_array_count; ++i, j += index_array_size )
        {
            for (int k = 0; k < index_array_size; ++k )
            {
                indices[ j + k ] = _index_arrays[ i ].indices[ k ];
            }
        }

        _index_uniform_buffer = kege::Graphics::createUniformBuffer
        (
            kege::STATIC_BUFFER, count * sizeof( indices[0] ), indices.data()
        );

        _resource_set = Graphics::allocateResourceSet({0, "IndexBuffers"});
        Graphics::updateResourceSet( _resource_set, { _index_uniform_buffer });

        return (_resource_set);
    }

}
