//
//  mesh-circle.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/22/25.
//

#include "mesh-circle.hpp"

namespace kege{

    void generateCircleVertices( std::vector< Vertex >& vertices, const vec3& center, float radius, int segments )
    {
        float ux, uy, px, py;

        // calculate the size of the vertices
        segments = (segments<3) ?3 :segments;

        const float PI = 3.1415926535897932f; // Use a constant for PI
        const float angle_increment = (2.0f * PI) / segments; // Calculate angle increment
        float angle = 0.0f;

        vertices.resize(segments + 1); // Pre-allocate memory for efficiency
        vertices[0].position = center;

        for (int i = 1; i <= segments; ++i)
        {
            ux = cos(angle);
            uy = sin(angle);

            // Calculate the x and y coordinates of the vertex
            px = radius * cosf(angle);
            py = radius * sinf(angle);

            // Create the vertex, adding the center offset
            vertices[i].position = center + vec3(px, py, 0.0f);
            vertices[i].texcoord = vec2( ux, uy );
            vertices[i].normal   = vec3( ux, 0.0f, uy );

            angle += angle_increment; // Increment the angle
        }
    }

    void generateCircleIndices( std::vector< uint32_t >& indices, int segments )
    {
        uint32_t k = 0;
        indices.resize(segments * 3);
        for (int i = 0; i < indices.size(); i += 3, k += 1)
        {
            indices[ i ] = 0;
            indices[i+1] = 1+k;
            indices[i+2] = 2+k;
        }
        indices[indices.size() - 1] = 1;
    }

    CircleMesh::CircleMesh( const vec3& center, float radius, int segments )
    {
        generateCircleVertices( vertices, center, radius, segments );
        generateCircleIndices( indices, segments );
    }

}
