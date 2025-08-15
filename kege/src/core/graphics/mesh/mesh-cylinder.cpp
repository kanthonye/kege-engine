//
//  mesh-cylinder.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/26/24.
//

#include "mesh-cylinder.hpp"

namespace kege{

    std::vector< Vertex > generateCylinderVertices( float radius, uint32_t height, uint32_t columns )
    {
        float vx,vy, nx,ny, rad;
        uint32_t index;
        uint32_t column_index;
        float half_height = (height/2.0f);
        const float segments = 6.28318530718 / (float)columns;
        columns = (columns<3) ? 3 : columns;

        std::vector< Vertex > vertices;
        vertices.resize( 4 * columns + 2 );

        // VERTEX ~ TOP CENTER
        vertices[ 0 ].position = kege::vec3(0.0f, 0.0f, half_height);
        vertices[ 0 ].normal   = kege::vec3(0.0f, 0.0f, 1.0);
        vertices[ 0 ].texcoord = kege::vec2(0.0f, 0.0f);

        // VERTEX ~ BOTTOM CENTER
        vertices[ 1 ].position = kege::vec3(0.0f, 0.0f, -half_height);
        vertices[ 1 ].normal   = kege::vec3(0.0f, 0.0f, -1.0);
        vertices[ 1 ].texcoord = kege::vec2(1.0f, 1.0f);

        for (column_index = 0; column_index < columns; column_index++ )
        {
            rad = segments * column_index;
            nx = cosf(rad);
            ny = sinf(rad);
            vx = nx*radius;
            vy = ny*radius;

            // VERTEX ~ TOP CIRCLE
            index = column_index * 4 + 2;
            vertices[ index ].position = kege::vec3( vx, vy, half_height);
            vertices[ index ].normal   = kege::vec3( 0.0f, 0.0f, 1.0);
            vertices[ index ].texcoord = kege::vec2( nx, ny);

            // VERTEX ~ BODY
            index++;
            vertices[ index ].position = kege::vec3( vx, vy, half_height);
            vertices[ index ].normal   = kege::vec3( nx, ny, 0.0f);
            vertices[ index ].texcoord = kege::vec2( nx, ny);

            index++;
            vertices[ index ].position = kege::vec3( vx, vy,-half_height);
            vertices[ index ].normal   = kege::vec3( nx, ny, 0.0f);
            vertices[ index ].texcoord = kege::vec2( 1.0f-nx, 1.0f-ny);

            // VERTEX ~ BOTTOM CIRCLE
            index++;
            vertices[ index ].position = kege::vec3( vx, vy,-half_height);
            vertices[ index ].normal   = kege::vec3( 0.0f, 0.0f, -1.0);
            vertices[ index ].texcoord = kege::vec2( 1.0f-nx, 1.0f-ny);
        }

        return vertices;
    }

    std::vector< uint32_t > generateCylinderIndices( uint32_t columns )
    {
        uint32_t v[4];
        uint32_t column_index;
        uint32_t col = 0;
        Vec3< uint32_t > triangles[4];
        columns = (columns<3) ? 3 : columns;

        std::vector< uint32_t > indices;
        indices.resize( 3 * columns + 3 * columns + 6 * columns );
        memset( indices.data(), 0, 4*indices.size() );
//        for (column_index = 0; column_index < columns; ++column_index)
        for (column_index = 0; column_index < columns; ++column_index, col += 12)
        {
            // top column triangle
            v[0]  = column_index * 4 + 2;
            triangles[0] = {col + 0, col + 1, col + 2};
            indices[ triangles[0].x ] = 0;
            indices[ triangles[0].y ] = v[0] + 4;
            indices[ triangles[0].z ] = v[0];

            // middle column triangle 1
            v[1] = v[0] + 1;
            triangles[1] = {col + 3, col + 4, col + 5};
            indices[ triangles[1].x ] = v[1];
            indices[ triangles[1].y ] = v[1] + 1;
            indices[ triangles[1].z ] = v[1] + 4;

            // middle column triangle 2
            v[2] = v[0] + 2;
            triangles[2] = {col + 6, col + 7, col + 8};
            indices[ triangles[2].x ] = v[2];
            indices[ triangles[2].y ] = v[2] + 4;
            indices[ triangles[2].z ] = v[2] + 3;

            // bottom column triangle
            v[3] = v[0] + 3;
            triangles[3] = {col + 9, col + 10, col + 11};
            indices[ triangles[3].x ] = 1;
            indices[ triangles[3].y ] = v[3];
            indices[ triangles[3].z ] = v[3] + 4;
        }
        indices[ triangles[0].y ] = 2;
        indices[ triangles[1].z ] = 4;
        indices[ triangles[2].x ] = 4;
        indices[ triangles[2].y ] = 3;
        indices[ triangles[3].z ] = 5;
        return indices;
    }

    CylinderMesh::CylinderMesh( float radius, uint32_t height, uint32_t columns )
    {
        vertices = generateCylinderVertices( radius, height, columns );
        indices = generateCylinderIndices( columns );
    }

}
