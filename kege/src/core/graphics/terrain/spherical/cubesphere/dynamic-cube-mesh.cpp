//
//  dynamic-cube-mesh.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/30/25.
//

#include "dynamic-cube-mesh.hpp"

namespace kege{

    DynamicCubeMesh::DynamicCubeMesh( float scale )
    {
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
            face_axies[ face ][ 0 ] = kege::normalize( cube_face_quads[ face ][ 3 ] - cube_face_quads[ face ][ 2 ] );
            face_axies[ face ][ 1 ] = kege::normalize( cube_face_quads[ face ][ 1 ] - cube_face_quads[ face ][ 2 ] );
            face_axies[ face ][ 2 ] = kege::cross( face_axies[ face ][ 0 ], face_axies[ face ][ 1 ] );

            int vertex_index = 0;
            for (float y = -1.f; y <= 1.f; y += 1.f )
            {
                for (float x = -1.f; x <= 1.f; x += 1.f )
                {
                    face_vertices[ face ].data[ vertex_index ].x = x * face_axies[ face ][ 0 ].x + y * face_axies[ face ][ 1 ].x;
                    face_vertices[ face ].data[ vertex_index ].y = x * face_axies[ face ][ 0 ].y + y * face_axies[ face ][ 1 ].y;
                    face_vertices[ face ].data[ vertex_index ].z = x * face_axies[ face ][ 0 ].z + y * face_axies[ face ][ 1 ].z;
                    face_vertices[ face ].data[ vertex_index ].w = scale;
                    vertex_index++;
                }
            }
        }

        // zero composite indices
        memset( face_indices, 0, sizeof(face_indices) );

        uint16_t i = 0; // 0000 = 0
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\       /|
        face_indices[ i ].data[  2 ] =  2; // | \     / |
        face_indices[ i ].data[  3 ] =  2; // |  \   /  |
        face_indices[ i ].data[  4 ] =  4; // |   \ /   |
        face_indices[ i ].data[  5 ] =  8; // 3    4    5
        face_indices[ i ].data[  6 ] =  8; // |   / \   |
        face_indices[ i ].data[  7 ] =  4; // |  /   \  |
        face_indices[ i ].data[  8 ] =  6; // | /     \ |
        face_indices[ i ].data[  9 ] =  6; // |/       \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  0;
        face_indices[ i ].draw_count = 12;

        i = 1; // 0001 = 1
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\   |   /|
        face_indices[ i ].data[  2 ] =  1; // | \  |  / |
        face_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        face_indices[ i ].data[  4 ] =  4; // |   \|/   |
        face_indices[ i ].data[  5 ] =  2; // 3    4    5
        face_indices[ i ].data[  6 ] =  2; // |   / \   |
        face_indices[ i ].data[  7 ] =  4; // |  /   \  |
        face_indices[ i ].data[  8 ] =  8; // | /     \ |
        face_indices[ i ].data[  9 ] =  8; // |/       \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  6;
        face_indices[ i ].data[ 12 ] =  6;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  0;
        face_indices[ i ].draw_count = 15;

        i = 2; // 0010 = 2
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\       /|
        face_indices[ i ].data[  2 ] =  2; // | \     / |
        face_indices[ i ].data[  3 ] =  2; // |  \   /  |
        face_indices[ i ].data[  4 ] =  4; // |   \ /   |
        face_indices[ i ].data[  5 ] =  5; // 3    4----5
        face_indices[ i ].data[  6 ] =  5; // |   / \   |
        face_indices[ i ].data[  7 ] =  4; // |  /   \  |
        face_indices[ i ].data[  8 ] =  8; // | /     \ |
        face_indices[ i ].data[  9 ] =  8; // |/       \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  6;
        face_indices[ i ].data[ 12 ] =  6;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  0;
        face_indices[ i ].draw_count = 15;

        i = 3; // 0011 = 3
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\   |   /|
        face_indices[ i ].data[  2 ] =  1; // | \  |  / |
        face_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        face_indices[ i ].data[  4 ] =  4; // |   \|/   |
        face_indices[ i ].data[  5 ] =  2; // 3    4----5
        face_indices[ i ].data[  6 ] =  2; // |   / \   |
        face_indices[ i ].data[  7 ] =  4; // |  /   \  |
        face_indices[ i ].data[  8 ] =  4; // | /     \ |
        face_indices[ i ].data[  9 ] =  5; // |/       \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  8;
        face_indices[ i ].data[ 12 ] =  8;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  6;
        face_indices[ i ].data[ 15 ] =  6;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  0;
        face_indices[ i ].draw_count = 18;

        i = 4; // 0100 = 4
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\       /|
        face_indices[ i ].data[  2 ] =  2; // | \     / |
        face_indices[ i ].data[  3 ] =  2; // |  \   /  |
        face_indices[ i ].data[  4 ] =  4; // |   \ /   |
        face_indices[ i ].data[  5 ] =  8; // 3    4    5
        face_indices[ i ].data[  6 ] =  8; // |   /|\   |
        face_indices[ i ].data[  7 ] =  4; // |  / | \  |
        face_indices[ i ].data[  8 ] =  7; // | /  |  \ |
        face_indices[ i ].data[  9 ] =  7; // |/   |   \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  6;
        face_indices[ i ].data[ 12 ] =  6;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  0;
        face_indices[ i ].draw_count = 15;

        i = 5; // 0101 = 5
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\   |   /|
        face_indices[ i ].data[  2 ] =  1; // | \  |  / |
        face_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        face_indices[ i ].data[  4 ] =  4; // |   \|/   |
        face_indices[ i ].data[  5 ] =  2; // 3    4    5
        face_indices[ i ].data[  6 ] =  2; // |   /|\   |
        face_indices[ i ].data[  7 ] =  4; // |  / | \  |
        face_indices[ i ].data[  8 ] =  8; // | /  |  \ |
        face_indices[ i ].data[  9 ] =  8; // |/   |   \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  7;
        face_indices[ i ].data[ 12 ] =  7;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  6;
        face_indices[ i ].data[ 15 ] =  6;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  0;
        face_indices[ i ].draw_count = 18;

        i = 6; // 0110 = 6
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\       /|
        face_indices[ i ].data[  2 ] =  2; // | \     / |
        face_indices[ i ].data[  3 ] =  2; // |  \   /  |
        face_indices[ i ].data[  4 ] =  4; // |   \ /   |
        face_indices[ i ].data[  5 ] =  5; // 3    4----5
        face_indices[ i ].data[  6 ] =  5; // |   /|\   |
        face_indices[ i ].data[  7 ] =  4; // |  / | \  |
        face_indices[ i ].data[  8 ] =  8; // | /  |  \ |
        face_indices[ i ].data[  9 ] =  8; // |/   |   \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  7;
        face_indices[ i ].data[ 12 ] =  7;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  6;
        face_indices[ i ].data[ 15 ] =  6;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  0;
        face_indices[ i ].draw_count = 18;

        i = 7; // 0111 = 7
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\   |   /|
        face_indices[ i ].data[  2 ] =  1; // | \  |  / |
        face_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        face_indices[ i ].data[  4 ] =  4; // |   \|/   |
        face_indices[ i ].data[  5 ] =  2; // 3    4----5
        face_indices[ i ].data[  6 ] =  2; // |   /|\   |
        face_indices[ i ].data[  7 ] =  4; // |  / | \  |
        face_indices[ i ].data[  8 ] =  5; // | /  |  \ |
        face_indices[ i ].data[  9 ] =  5; // |/   |   \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  8;
        face_indices[ i ].data[ 12 ] =  8;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  7;
        face_indices[ i ].data[ 15 ] =  7;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  6;
        face_indices[ i ].data[ 18 ] =  6;
        face_indices[ i ].data[ 19 ] =  4;
        face_indices[ i ].data[ 20 ] =  0;
        face_indices[ i ].draw_count = 21;

        i = 8; // 1000 = 8
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\       /|
        face_indices[ i ].data[  2 ] =  2; // | \     / |
        face_indices[ i ].data[  3 ] =  2; // |  \   /  |
        face_indices[ i ].data[  4 ] =  4; // |   \ /   |
        face_indices[ i ].data[  5 ] =  8; // 3----4    5
        face_indices[ i ].data[  6 ] =  8; // |   / \   |
        face_indices[ i ].data[  7 ] =  4; // |  /   \  |
        face_indices[ i ].data[  8 ] =  6; // | /     \ |
        face_indices[ i ].data[  9 ] =  6; // |/       \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  3;
        face_indices[ i ].data[ 12 ] =  3;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  0;
        face_indices[ i ].data[ 15 ] =  0;
        face_indices[ i ].data[ 16 ] =  0;
        face_indices[ i ].data[ 17 ] =  0;
        face_indices[ i ].data[ 18 ] =  0;
        face_indices[ i ].data[ 19 ] =  0;
        face_indices[ i ].data[ 20 ] =  0;
        face_indices[ i ].draw_count = 15;

        i = 9; // 1001 = 9
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\   |   /|
        face_indices[ i ].data[  2 ] =  1; // | \  |  / |
        face_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        face_indices[ i ].data[  4 ] =  4; // |   \|/   |
        face_indices[ i ].data[  5 ] =  2; // 3----4    5
        face_indices[ i ].data[  6 ] =  2; // |   / \   |
        face_indices[ i ].data[  7 ] =  4; // |  /   \  |
        face_indices[ i ].data[  8 ] =  8; // | /     \ |
        face_indices[ i ].data[  9 ] =  8; // |/       \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  6;
        face_indices[ i ].data[ 12 ] =  6;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  3;
        face_indices[ i ].data[ 15 ] =  3;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  0;
        face_indices[ i ].data[ 18 ] =  0;
        face_indices[ i ].data[ 19 ] =  0;
        face_indices[ i ].data[ 20 ] =  0;
        face_indices[ i ].draw_count = 18;

        i = 10; // 1010 = 10
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\       /|
        face_indices[ i ].data[  2 ] =  2; // | \     / |
        face_indices[ i ].data[  3 ] =  2; // |  \   /  |
        face_indices[ i ].data[  4 ] =  4; // |   \ /   |
        face_indices[ i ].data[  5 ] =  5; // 3----4----5
        face_indices[ i ].data[  6 ] =  5; // |   / \   |
        face_indices[ i ].data[  7 ] =  4; // |  /   \  |
        face_indices[ i ].data[  8 ] =  8; // | /     \ |
        face_indices[ i ].data[  9 ] =  8; // |/       \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  6;
        face_indices[ i ].data[ 12 ] =  6;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  3;
        face_indices[ i ].data[ 15 ] =  3;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  0;
        face_indices[ i ].data[ 18 ] =  0;
        face_indices[ i ].data[ 19 ] =  0;
        face_indices[ i ].data[ 20 ] =  0;
        face_indices[ i ].draw_count = 18;

        i = 11; // 1011 = 11
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\   |   /|
        face_indices[ i ].data[  2 ] =  1; // | \  |  / |
        face_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        face_indices[ i ].data[  4 ] =  4; // |   \|/   |
        face_indices[ i ].data[  5 ] =  2; // 3----4----5
        face_indices[ i ].data[  6 ] =  2; // |   / \   |
        face_indices[ i ].data[  7 ] =  4; // |  /   \  |
        face_indices[ i ].data[  8 ] =  5; // | /     \ |
        face_indices[ i ].data[  9 ] =  5; // |/       \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  8;
        face_indices[ i ].data[ 12 ] =  8;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  6;
        face_indices[ i ].data[ 15 ] =  6;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  3;
        face_indices[ i ].data[ 18 ] =  3;
        face_indices[ i ].data[ 19 ] =  4;
        face_indices[ i ].data[ 20 ] =  0;
        face_indices[ i ].draw_count = 21;

        i = 12; // 1100 = 12
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\       /|
        face_indices[ i ].data[  2 ] =  2; // | \     / |
        face_indices[ i ].data[  3 ] =  2; // |  \   /  |
        face_indices[ i ].data[  4 ] =  4; // |   \ /   |
        face_indices[ i ].data[  5 ] =  8; // 3----4    5
        face_indices[ i ].data[  6 ] =  8; // |   /|\   |
        face_indices[ i ].data[  7 ] =  4; // |  / | \  |
        face_indices[ i ].data[  8 ] =  7; // | /  |  \ |
        face_indices[ i ].data[  9 ] =  7; // |/   |   \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  6;
        face_indices[ i ].data[ 12 ] =  6;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  3;
        face_indices[ i ].data[ 15 ] =  3;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  0;
        face_indices[ i ].data[ 18 ] =  0;
        face_indices[ i ].data[ 19 ] =  0;
        face_indices[ i ].data[ 20 ] =  0;
        face_indices[ i ].draw_count = 18;

        i = 13; // 1101 = 13
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\   |   /|
        face_indices[ i ].data[  2 ] =  1; // | \  |  / |
        face_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        face_indices[ i ].data[  4 ] =  4; // |   \|/   |
        face_indices[ i ].data[  5 ] =  2; // 3----4    5
        face_indices[ i ].data[  6 ] =  2; // |   /|\   |
        face_indices[ i ].data[  7 ] =  4; // |  / | \  |
        face_indices[ i ].data[  8 ] =  8; // | /  |  \ |
        face_indices[ i ].data[  9 ] =  8; // |/   |   \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  7;
        face_indices[ i ].data[ 12 ] =  7;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  6;
        face_indices[ i ].data[ 15 ] =  6;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  3;
        face_indices[ i ].data[ 18 ] =  3;
        face_indices[ i ].data[ 19 ] =  4;
        face_indices[ i ].data[ 20 ] =  0;
        face_indices[ i ].draw_count = 21;

        i = 14; // 1110 = 14
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\       /|
        face_indices[ i ].data[  2 ] =  2; // | \     / |
        face_indices[ i ].data[  3 ] =  2; // |  \   /  |
        face_indices[ i ].data[  4 ] =  4; // |   \ /   |
        face_indices[ i ].data[  5 ] =  5; // 3----4----5
        face_indices[ i ].data[  6 ] =  5; // |   /|\   |
        face_indices[ i ].data[  7 ] =  4; // |  / | \  |
        face_indices[ i ].data[  8 ] =  8; // | /  |  \ |
        face_indices[ i ].data[  9 ] =  8; // |/   |   \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  7;
        face_indices[ i ].data[ 12 ] =  7;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  6;
        face_indices[ i ].data[ 15 ] =  6;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  3;
        face_indices[ i ].data[ 18 ] =  3;
        face_indices[ i ].data[ 19 ] =  4;
        face_indices[ i ].data[ 20 ] =  0;
        face_indices[ i ].draw_count = 21;

        i = 15; // 1111 = 15
        face_indices[ i ].data[  0 ] =  0; // 0----1----2
        face_indices[ i ].data[  1 ] =  4; // |\   |   /|
        face_indices[ i ].data[  2 ] =  1; // | \  |  / |
        face_indices[ i ].data[  3 ] =  1; // |  \ | /  |
        face_indices[ i ].data[  4 ] =  4; // |   \|/   |
        face_indices[ i ].data[  5 ] =  2; // 3----4----5
        face_indices[ i ].data[  6 ] =  2; // |   /|\   |
        face_indices[ i ].data[  7 ] =  4; // |  / | \  |
        face_indices[ i ].data[  8 ] =  5; // | /  |  \ |
        face_indices[ i ].data[  9 ] =  5; // |/   |   \|
        face_indices[ i ].data[ 10 ] =  4; // 6----7----8
        face_indices[ i ].data[ 11 ] =  8;
        face_indices[ i ].data[ 12 ] =  8;
        face_indices[ i ].data[ 13 ] =  4;
        face_indices[ i ].data[ 14 ] =  7;
        face_indices[ i ].data[ 15 ] =  7;
        face_indices[ i ].data[ 16 ] =  4;
        face_indices[ i ].data[ 17 ] =  6;
        face_indices[ i ].data[ 18 ] =  6;
        face_indices[ i ].data[ 19 ] =  4;
        face_indices[ i ].data[ 20 ] =  3;
        face_indices[ i ].data[ 21 ] =  3;
        face_indices[ i ].data[ 22 ] =  4;
        face_indices[ i ].data[ 23 ] =  0;
        face_indices[ i ].draw_count = 24;
    }

}
