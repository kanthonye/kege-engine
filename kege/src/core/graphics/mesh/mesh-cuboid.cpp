//
//  cuboid.cpp
//  new-engine
//
//  Created by Kenneth Esdaile on 2/18/24.
//

#include "../mesh/mesh-cuboid.hpp"
namespace kege{

    CuboidMesh::CuboidMesh( const kege::vec3& center, const kege::vec3& lengths )
    {
        const uint64_t vertex_count = 24;

        kege::vec3 p[ 8 ];
        p[0] = center + kege::vec3(  lengths.x,  lengths.y,  lengths.z );
        p[1] = center + kege::vec3( -lengths.x,  lengths.y,  lengths.z );
        p[2] = center + kege::vec3( -lengths.x, -lengths.y,  lengths.z );
        p[3] = center + kege::vec3(  lengths.x, -lengths.y,  lengths.z );

        p[4] = center + kege::vec3(  lengths.x,  lengths.y, -lengths.z );
        p[5] = center + kege::vec3( -lengths.x,  lengths.y, -lengths.z );
        p[6] = center + kege::vec3( -lengths.x, -lengths.y, -lengths.z );
        p[7] = center + kege::vec3(  lengths.x, -lengths.y, -lengths.z );

        //this->min_bound = p[6];
        //this->max_bound = p[0];

        kege::vec3 n[ 6 ] =
        {
            kege::vec3( 0.f, 0.f, 1.f),
            kege::vec3( 0.f, 0.f,-1.f),
            kege::vec3(-1.f, 0.f, 0.f),
            kege::vec3( 1.f, 0.f, 0.f),
            kege::vec3( 0.f, 1.f, 0.f),
            kege::vec3( 0.f,-1.f, 0.f)
        };

        kege::vec2 t[ 4 ] =
        {
            kege::vec2(1.0f, 1.0f),
            kege::vec2(0.0f, 1.0f),
            kege::vec2(0.0f, 0.0f),
            kege::vec2(0.0f, 1.0f)
        };

        uint32_t faces[6][4] =
        {
            { 0 , 1 , 2 , 3 }, // front plane
            { 4 , 5 , 6 , 7 }, // back plane
            { 1 , 5 , 6 , 2 }, // left plane
            { 4 , 0 , 3 , 7 }, // right plane
            { 4 , 5 , 1 , 0 }, // top plane
            { 3 , 2 , 6 , 7 }  // bottom plane
        };

        vertices.resize( vertex_count );
        for (int face=0, i, k; face<6; face++)
        {
            for ( i=0; i<4; i++)
            {
                k = face * 4 + i;
                vertices[ k ].position = p[ faces[ face ][ i ] ];
                vertices[ k ].normal   = n[ face ];
                vertices[ k ].texcoord = t[ i ];
            }
        }

        indices = std::vector< uint32_t >
        {
            // front plane
            0 , 1 , 2 ,
            2 , 3 , 0 ,
            // back plane
            5 , 4 , 7 ,
            7 , 6 , 5 ,
            // left plane
            8 , 9 , 10,
            10, 11, 8 ,
            // right plane
            12, 13, 14,
            14, 15, 12,
            // left plane
            16, 17, 18,
            18, 19, 16,
            // right plane
            20, 21, 22,
            22, 23, 20
        };
    }
    
}
