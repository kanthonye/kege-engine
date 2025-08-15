//
//  mesh-cone.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/16/25.
//

#include "mesh-cone.hpp"
namespace kege{
    
    std::vector<Vertex> generateConeVertices(float height, float radius, int columns)
    {
        std::vector<Vertex> vertices;

        // Validate input parameters
        if (height <= 0.0f || radius <= 0.0f || columns < 3) {
            return vertices; // Return empty mesh for invalid parameters
        }

        // Calculate cone properties
        float half_height = height * 0.5f;
        //float slant_length = std::sqrt(height * height + radius * radius);

        // Generate base vertices (circle)
        for (int i = 0; i <= columns; ++i) {
            float u = static_cast<float>(i) / columns;
            float angle = u * 2.0f * M_PI;

            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);

            Vertex vertex;
            vertex.position = kege::vec3(x, -half_height, z);
            vertex.texcoord = kege::vec2(u, 0.0f);
            vertex.normal = kege::vec3(0.0f, -1.0f, 0.0f); // Downward normal for base

            vertices.push_back(vertex);
        }

        // Generate side vertices
        for (int i = 0; i <= columns; ++i)
        {
            float u = static_cast<float>(i) / columns;
            float angle = u * 2.0f * M_PI;

            float x = std::cos(angle);
            float z = std::sin(angle);

            // Base vertex for sides
            Vertex baseVertex;
            baseVertex.position = kege::vec3(radius * x, -half_height, radius * z);
            baseVertex.texcoord = kege::vec2(u, 0.0f);

            // Normal calculation for sides (perpendicular to the slant)
            kege::vec3 normal = kege::normalize(kege::vec3(height * x, radius, height * z));
            baseVertex.normal = normal;

            vertices.push_back(baseVertex);

            // Apex vertex (all sides meet here)
            Vertex apexVertex;
            apexVertex.position = kege::vec3(0.0f, half_height, 0.0f);
            apexVertex.texcoord = kege::vec2(u, 1.0f);
            apexVertex.normal = normal; // Same normal as corresponding base vertex

            vertices.push_back(apexVertex);
        }

        // Generate center vertex for base (for proper base triangulation)
        Vertex centerVertex;
        centerVertex.position = kege::vec3(0.0f, -half_height, 0.0f);
        centerVertex.texcoord = kege::vec2(0.5f, 0.5f);
        centerVertex.normal = kege::vec3(0.0f, -1.0f, 0.0f);
        vertices.push_back(centerVertex);

        return vertices;
    }

    std::vector<unsigned int> generateConeIndices(int columns)
    {
        std::vector<unsigned int> indices;

        // Generate base indices (triangle fan)
        unsigned int centerIndex = (columns + 1) * 2 + 1;
        for (int i = 0; i < columns; ++i)
        {
            indices.push_back(centerIndex);
            indices.push_back(i + 1);
            indices.push_back(i);
        }

        // Generate side indices (triangles)
        unsigned int baseOffset = columns + 1;
        for (int i = 0; i < columns; ++i)
        {
            unsigned int baseIndex1 = baseOffset + i * 2;
            unsigned int baseIndex2 = baseOffset + (i + 1) * 2;
            unsigned int apexIndex = baseOffset + i * 2 + 1;

            indices.push_back(baseIndex1);
            indices.push_back(apexIndex);
            indices.push_back(baseIndex2);
        }

        return indices;
    }


    ConeMesh::ConeMesh( float radius, float height, uint32_t columns )
    {
        vertices = generateConeVertices( radius, height, columns );
        indices = generateConeIndices( columns );
    }
}
