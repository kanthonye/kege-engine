//
//  cube-mesh-shader-resource.cpp
//  kege
//
//  Created by Kenneth Esdaile on 11/9/24.
//

#include "cube-mesh-shader-resource.hpp"

namespace kege{

    const CubeMeshShaderResource* createQuadtreeCube( float scale )
    {
        static CubeMeshShaderResource cube;
        if ( cube.initialized )
        {
            return &cube;
        }

        kege::vec3 points[ 8 ];
        points[ 0 ] = kege::vec3(  scale,  scale,  scale );
        points[ 1 ] = kege::vec3( -scale,  scale,  scale );
        points[ 2 ] = kege::vec3( -scale, -scale,  scale );
        points[ 3 ] = kege::vec3(  scale, -scale,  scale );

        points[ 4 ] = kege::vec3(  scale,  scale, -scale );
        points[ 5 ] = kege::vec3( -scale,  scale, -scale );
        points[ 6 ] = kege::vec3( -scale, -scale, -scale );
        points[ 7 ] = kege::vec3(  scale, -scale, -scale );

        std::vector< kege::vec3 > cube_face_quads[ 6 ];
        cube_face_quads[ CUBE_FACE_FRONT ] = { points[ 0 ], points[ 1 ], points[ 2 ], points[ 3 ] };
        cube_face_quads[ CUBE_FACE_BACK  ] = { points[ 5 ], points[ 4 ], points[ 7 ], points[ 6 ] };
        cube_face_quads[ CUBE_FACE_LEFT  ] = { points[ 1 ], points[ 5 ], points[ 6 ], points[ 2 ] };
        cube_face_quads[ CUBE_FACE_RIGHT ] = { points[ 4 ], points[ 0 ], points[ 3 ], points[ 7 ] };
        cube_face_quads[ CUBE_FACE_ABOVE ] = { points[ 2 ], points[ 6 ], points[ 7 ], points[ 3 ] };
        cube_face_quads[ CUBE_FACE_BELOW ] = { points[ 5 ], points[ 1 ], points[ 0 ], points[ 4 ] };

        for (int face = 0; face < MAX_CUBE_FACES; face++ )
        {
            cube.axies[ face ][ 0 ] = kege::normalize( cube_face_quads[ face ][ 3 ] - cube_face_quads[ face ][ 2 ] );
            cube.axies[ face ][ 1 ] = kege::normalize( cube_face_quads[ face ][ 1 ] - cube_face_quads[ face ][ 2 ] );
            cube.axies[ face ][ 2 ] = kege::cross( cube.axies[ face ][ 0 ], cube.axies[ face ][ 1 ] );

            int vertex_index = 0;
            for (float y = -1.f; y <= 1.f; y += 1.f )
            {
                for (float x = -1.f; x <= 1.f; x += 1.f )
                {
                    cube.composite_vertices[ face ].data[ vertex_index ].x = x * cube.axies[ face ][ 0 ].x + y * cube.axies[ face ][ 1 ].x;// + cube.axies[ face ][ 2 ].x;
                    cube.composite_vertices[ face ].data[ vertex_index ].y = x * cube.axies[ face ][ 0 ].y + y * cube.axies[ face ][ 1 ].y;// + cube.axies[ face ][ 2 ].y;
                    cube.composite_vertices[ face ].data[ vertex_index ].z = x * cube.axies[ face ][ 0 ].z + y * cube.axies[ face ][ 1 ].z;// + cube.axies[ face ][ 2 ].z;
                    cube.composite_vertices[ face ].data[ vertex_index ].w = scale;
                    vertex_index++;
                }
            }
        }

        // zero composite indices
        memset( cube.composite_indices, 0, sizeof(cube.composite_indices) );

        uint16_t i = 0; // 0000 = 0
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\       /|
        cube.composite_indices[ i ].data[  2 ] =  2; // | \     / |
        cube.composite_indices[ i ].data[  3 ] =  2; // |  \   /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \ /   |
        cube.composite_indices[ i ].data[  5 ] =  8; // 3    4    5
        cube.composite_indices[ i ].data[  6 ] =  8; // |   / \   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  /   \  |
        cube.composite_indices[ i ].data[  8 ] =  6; // | /     \ |
        cube.composite_indices[ i ].data[  9 ] =  6; // |/       \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  0;
        cube.composite_indices[ i ].draw_count = 12;

        i = 1; // 0001 = 1
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\   |   /|
        cube.composite_indices[ i ].data[  2 ] =  1; // | \  |  / |
        cube.composite_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \|/   |
        cube.composite_indices[ i ].data[  5 ] =  2; // 3    4    5
        cube.composite_indices[ i ].data[  6 ] =  2; // |   / \   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  /   \  |
        cube.composite_indices[ i ].data[  8 ] =  8; // | /     \ |
        cube.composite_indices[ i ].data[  9 ] =  8; // |/       \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  6;
        cube.composite_indices[ i ].data[ 12 ] =  6;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  0;
        cube.composite_indices[ i ].draw_count = 15;

        i = 2; // 0010 = 2
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\       /|
        cube.composite_indices[ i ].data[  2 ] =  2; // | \     / |
        cube.composite_indices[ i ].data[  3 ] =  2; // |  \   /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \ /   |
        cube.composite_indices[ i ].data[  5 ] =  5; // 3    4----5
        cube.composite_indices[ i ].data[  6 ] =  5; // |   / \   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  /   \  |
        cube.composite_indices[ i ].data[  8 ] =  8; // | /     \ |
        cube.composite_indices[ i ].data[  9 ] =  8; // |/       \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  6;
        cube.composite_indices[ i ].data[ 12 ] =  6;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  0;
        cube.composite_indices[ i ].draw_count = 15;

        i = 3; // 0011 = 3
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\   |   /|
        cube.composite_indices[ i ].data[  2 ] =  1; // | \  |  / |
        cube.composite_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \|/   |
        cube.composite_indices[ i ].data[  5 ] =  2; // 3    4----5
        cube.composite_indices[ i ].data[  6 ] =  2; // |   / \   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  /   \  |
        cube.composite_indices[ i ].data[  8 ] =  4; // | /     \ |
        cube.composite_indices[ i ].data[  9 ] =  5; // |/       \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  8;
        cube.composite_indices[ i ].data[ 12 ] =  8;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  6;
        cube.composite_indices[ i ].data[ 15 ] =  6;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  0;
        cube.composite_indices[ i ].draw_count = 18;

        i = 4; // 0100 = 4
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\       /|
        cube.composite_indices[ i ].data[  2 ] =  2; // | \     / |
        cube.composite_indices[ i ].data[  3 ] =  2; // |  \   /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \ /   |
        cube.composite_indices[ i ].data[  5 ] =  8; // 3    4    5
        cube.composite_indices[ i ].data[  6 ] =  8; // |   /|\   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  / | \  |
        cube.composite_indices[ i ].data[  8 ] =  7; // | /  |  \ |
        cube.composite_indices[ i ].data[  9 ] =  7; // |/   |   \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  6;
        cube.composite_indices[ i ].data[ 12 ] =  6;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  0;
        cube.composite_indices[ i ].draw_count = 15;

        i = 5; // 0101 = 5
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\   |   /|
        cube.composite_indices[ i ].data[  2 ] =  1; // | \  |  / |
        cube.composite_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \|/   |
        cube.composite_indices[ i ].data[  5 ] =  2; // 3    4    5
        cube.composite_indices[ i ].data[  6 ] =  2; // |   /|\   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  / | \  |
        cube.composite_indices[ i ].data[  8 ] =  8; // | /  |  \ |
        cube.composite_indices[ i ].data[  9 ] =  8; // |/   |   \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  7;
        cube.composite_indices[ i ].data[ 12 ] =  7;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  6;
        cube.composite_indices[ i ].data[ 15 ] =  6;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  0;
        cube.composite_indices[ i ].draw_count = 18;

        i = 6; // 0110 = 6
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\       /|
        cube.composite_indices[ i ].data[  2 ] =  2; // | \     / |
        cube.composite_indices[ i ].data[  3 ] =  2; // |  \   /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \ /   |
        cube.composite_indices[ i ].data[  5 ] =  5; // 3    4----5
        cube.composite_indices[ i ].data[  6 ] =  5; // |   /|\   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  / | \  |
        cube.composite_indices[ i ].data[  8 ] =  8; // | /  |  \ |
        cube.composite_indices[ i ].data[  9 ] =  8; // |/   |   \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  7;
        cube.composite_indices[ i ].data[ 12 ] =  7;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  6;
        cube.composite_indices[ i ].data[ 15 ] =  6;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  0;
        cube.composite_indices[ i ].draw_count = 18;

        i = 7; // 0111 = 7
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\   |   /|
        cube.composite_indices[ i ].data[  2 ] =  1; // | \  |  / |
        cube.composite_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \|/   |
        cube.composite_indices[ i ].data[  5 ] =  2; // 3    4----5
        cube.composite_indices[ i ].data[  6 ] =  2; // |   /|\   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  / | \  |
        cube.composite_indices[ i ].data[  8 ] =  5; // | /  |  \ |
        cube.composite_indices[ i ].data[  9 ] =  5; // |/   |   \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  8;
        cube.composite_indices[ i ].data[ 12 ] =  8;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  7;
        cube.composite_indices[ i ].data[ 15 ] =  7;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  6;
        cube.composite_indices[ i ].data[ 18 ] =  6;
        cube.composite_indices[ i ].data[ 19 ] =  4;
        cube.composite_indices[ i ].data[ 20 ] =  0;
        cube.composite_indices[ i ].draw_count = 21;

        i = 8; // 1000 = 8
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\       /|
        cube.composite_indices[ i ].data[  2 ] =  2; // | \     / |
        cube.composite_indices[ i ].data[  3 ] =  2; // |  \   /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \ /   |
        cube.composite_indices[ i ].data[  5 ] =  8; // 3----4    5
        cube.composite_indices[ i ].data[  6 ] =  8; // |   / \   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  /   \  |
        cube.composite_indices[ i ].data[  8 ] =  6; // | /     \ |
        cube.composite_indices[ i ].data[  9 ] =  6; // |/       \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  3;
        cube.composite_indices[ i ].data[ 12 ] =  3;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  0;
        cube.composite_indices[ i ].data[ 15 ] =  0;
        cube.composite_indices[ i ].data[ 16 ] =  0;
        cube.composite_indices[ i ].data[ 17 ] =  0;
        cube.composite_indices[ i ].data[ 18 ] =  0;
        cube.composite_indices[ i ].data[ 19 ] =  0;
        cube.composite_indices[ i ].data[ 20 ] =  0;
        cube.composite_indices[ i ].draw_count = 15;

        i = 9; // 1001 = 9
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\   |   /|
        cube.composite_indices[ i ].data[  2 ] =  1; // | \  |  / |
        cube.composite_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \|/   |
        cube.composite_indices[ i ].data[  5 ] =  2; // 3----4    5
        cube.composite_indices[ i ].data[  6 ] =  2; // |   / \   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  /   \  |
        cube.composite_indices[ i ].data[  8 ] =  8; // | /     \ |
        cube.composite_indices[ i ].data[  9 ] =  8; // |/       \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  6;
        cube.composite_indices[ i ].data[ 12 ] =  6;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  3;
        cube.composite_indices[ i ].data[ 15 ] =  3;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  0;
        cube.composite_indices[ i ].data[ 18 ] =  0;
        cube.composite_indices[ i ].data[ 19 ] =  0;
        cube.composite_indices[ i ].data[ 20 ] =  0;
        cube.composite_indices[ i ].draw_count = 18;

        i = 10; // 1010 = 10
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\       /|
        cube.composite_indices[ i ].data[  2 ] =  2; // | \     / |
        cube.composite_indices[ i ].data[  3 ] =  2; // |  \   /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \ /   |
        cube.composite_indices[ i ].data[  5 ] =  5; // 3----4----5
        cube.composite_indices[ i ].data[  6 ] =  5; // |   / \   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  /   \  |
        cube.composite_indices[ i ].data[  8 ] =  8; // | /     \ |
        cube.composite_indices[ i ].data[  9 ] =  8; // |/       \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  6;
        cube.composite_indices[ i ].data[ 12 ] =  6;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  3;
        cube.composite_indices[ i ].data[ 15 ] =  3;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  0;
        cube.composite_indices[ i ].data[ 18 ] =  0;
        cube.composite_indices[ i ].data[ 19 ] =  0;
        cube.composite_indices[ i ].data[ 20 ] =  0;
        cube.composite_indices[ i ].draw_count = 18;

        i = 11; // 1011 = 11
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\   |   /|
        cube.composite_indices[ i ].data[  2 ] =  1; // | \  |  / |
        cube.composite_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \|/   |
        cube.composite_indices[ i ].data[  5 ] =  2; // 3----4----5
        cube.composite_indices[ i ].data[  6 ] =  2; // |   / \   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  /   \  |
        cube.composite_indices[ i ].data[  8 ] =  5; // | /     \ |
        cube.composite_indices[ i ].data[  9 ] =  5; // |/       \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  8;
        cube.composite_indices[ i ].data[ 12 ] =  8;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  6;
        cube.composite_indices[ i ].data[ 15 ] =  6;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  3;
        cube.composite_indices[ i ].data[ 18 ] =  3;
        cube.composite_indices[ i ].data[ 19 ] =  4;
        cube.composite_indices[ i ].data[ 20 ] =  0;
        cube.composite_indices[ i ].draw_count = 21;

        i = 12; // 1100 = 12
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\       /|
        cube.composite_indices[ i ].data[  2 ] =  2; // | \     / |
        cube.composite_indices[ i ].data[  3 ] =  2; // |  \   /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \ /   |
        cube.composite_indices[ i ].data[  5 ] =  8; // 3----4    5
        cube.composite_indices[ i ].data[  6 ] =  8; // |   /|\   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  / | \  |
        cube.composite_indices[ i ].data[  8 ] =  7; // | /  |  \ |
        cube.composite_indices[ i ].data[  9 ] =  7; // |/   |   \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  6;
        cube.composite_indices[ i ].data[ 12 ] =  6;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  3;
        cube.composite_indices[ i ].data[ 15 ] =  3;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  0;
        cube.composite_indices[ i ].data[ 18 ] =  0;
        cube.composite_indices[ i ].data[ 19 ] =  0;
        cube.composite_indices[ i ].data[ 20 ] =  0;
        cube.composite_indices[ i ].draw_count = 18;

        i = 13; // 1101 = 13
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\   |   /|
        cube.composite_indices[ i ].data[  2 ] =  1; // | \  |  / |
        cube.composite_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \|/   |
        cube.composite_indices[ i ].data[  5 ] =  2; // 3----4    5
        cube.composite_indices[ i ].data[  6 ] =  2; // |   /|\   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  / | \  |
        cube.composite_indices[ i ].data[  8 ] =  8; // | /  |  \ |
        cube.composite_indices[ i ].data[  9 ] =  8; // |/   |   \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  7;
        cube.composite_indices[ i ].data[ 12 ] =  7;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  6;
        cube.composite_indices[ i ].data[ 15 ] =  6;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  3;
        cube.composite_indices[ i ].data[ 18 ] =  3;
        cube.composite_indices[ i ].data[ 19 ] =  4;
        cube.composite_indices[ i ].data[ 20 ] =  0;
        cube.composite_indices[ i ].draw_count = 21;

        i = 14; // 1110 = 14
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\       /|
        cube.composite_indices[ i ].data[  2 ] =  2; // | \     / |
        cube.composite_indices[ i ].data[  3 ] =  2; // |  \   /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \ /   |
        cube.composite_indices[ i ].data[  5 ] =  5; // 3----4----5
        cube.composite_indices[ i ].data[  6 ] =  5; // |   /|\   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  / | \  |
        cube.composite_indices[ i ].data[  8 ] =  8; // | /  |  \ |
        cube.composite_indices[ i ].data[  9 ] =  8; // |/   |   \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  7;
        cube.composite_indices[ i ].data[ 12 ] =  7;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  6;
        cube.composite_indices[ i ].data[ 15 ] =  6;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  3;
        cube.composite_indices[ i ].data[ 18 ] =  3;
        cube.composite_indices[ i ].data[ 19 ] =  4;
        cube.composite_indices[ i ].data[ 20 ] =  0;
        cube.composite_indices[ i ].draw_count = 21;

        i = 15; // 1111 = 15
        cube.composite_indices[ i ].data[  0 ] =  0; // 0----1----2
        cube.composite_indices[ i ].data[  1 ] =  4; // |\   |   /|
        cube.composite_indices[ i ].data[  2 ] =  1; // | \  |  / |
        cube.composite_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        cube.composite_indices[ i ].data[  4 ] =  4; // |   \|/   |
        cube.composite_indices[ i ].data[  5 ] =  2; // 3----4----5
        cube.composite_indices[ i ].data[  6 ] =  2; // |   /|\   |
        cube.composite_indices[ i ].data[  7 ] =  4; // |  / | \  |
        cube.composite_indices[ i ].data[  8 ] =  5; // | /  |  \ |
        cube.composite_indices[ i ].data[  9 ] =  5; // |/   |   \|
        cube.composite_indices[ i ].data[ 10 ] =  4; // 6----7----8
        cube.composite_indices[ i ].data[ 11 ] =  8;
        cube.composite_indices[ i ].data[ 12 ] =  8;
        cube.composite_indices[ i ].data[ 13 ] =  4;
        cube.composite_indices[ i ].data[ 14 ] =  7;
        cube.composite_indices[ i ].data[ 15 ] =  7;
        cube.composite_indices[ i ].data[ 16 ] =  4;
        cube.composite_indices[ i ].data[ 17 ] =  6;
        cube.composite_indices[ i ].data[ 18 ] =  6;
        cube.composite_indices[ i ].data[ 19 ] =  4;
        cube.composite_indices[ i ].data[ 20 ] =  3;
        cube.composite_indices[ i ].data[ 21 ] =  3;
        cube.composite_indices[ i ].data[ 22 ] =  4;
        cube.composite_indices[ i ].data[ 23 ] =  0;
        cube.composite_indices[ i ].draw_count = 24;

        kege::Vec4< uint32_t > cube_face_indices[ 16 ][ 24 ];
        for (int j,i = 0; i < 16; i++ )
        {
            for (j = 0; j < 24; j++ )
            {
                cube_face_indices[ i ][ j ].x = cube.composite_indices[ i ].data[ j ];
            }
        }

        size_t composite_vertices_size = sizeof( cube.composite_vertices );
        size_t cube_face_indices_size = sizeof( kege::Vec4< uint32_t > ) * 16 * 24;
        cube.composite_vertices_uniform_buffer = kege::UBO({ composite_vertices_size, cube.composite_vertices, kege::DYNAMIC_BUFFER });
        cube.composite_indices_uniform_buffer = kege::UBO({ cube_face_indices_size, cube_face_indices, kege::DYNAMIC_BUFFER });

        ShaderResourceLayout::create( cube.mesh_shader_resource, {{
            {
                {
                    DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    "PatchVertexBuffer", 1, 0,
                    VERTEX_SHADER, 1, 0
                },
                &cube.composite_vertices_uniform_buffer
            },
            {
                {
                    DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    "PatchIndexBuffer", 1, 1,
                    VERTEX_SHADER, 1, 0
                },
                &cube.composite_indices_uniform_buffer
            }
        }});
//        cube.mesh_shader_resource = ShaderResourceLayout
//        ({
//            {
//                "PatchVertexBuffer", // string name
//                cube.composite_vertices_uniform_buffer.id(), // resource_id
//                DESCRIPTOR_TYPE_UNIFORM_BUFFER, // DescriptorType
//                0, // uint32_t binding
//                1, // uint32_t count
//                1, // uint32_t set
//            },
//            {
//                "PatchIndexBuffer", // string name
//                cube.composite_indices_uniform_buffer.id(), // resource_id
//                DESCRIPTOR_TYPE_UNIFORM_BUFFER, // DescriptorType
//                1, // uint32_t binding
//                1, // uint32_t count
//                1, // uint32_t set
//            }
//        });

        cube.initialized = true;
        return &cube;
    }



}
