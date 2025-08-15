//
//  mesh-capsule.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/16/25.
//

#include "mesh-capsule.hpp"

namespace kege{

    std::vector<Vertex> generateCapsuleVertices(float height, float radius, int columns, int rings)
    {
        std::vector<Vertex> vertices;

        // Validate input parameters
        if (height <= 0.0f || radius <= 0.0f || columns < 3 || rings < 1)
        {
            return vertices; // Return empty mesh for invalid parameters
        }

        // Calculate capsule properties
        float halfHeight = (height - 2.0f * radius) * 0.5f;
        if (halfHeight < 0.0f) halfHeight = 0.0f; // Handle case where height <= 2*radius

        // Generate vertices
        for (int i = 0; i <= rings; ++i)
        {
            float v = static_cast<float>(i) / rings;
            float latitude = v * M_PI; // 0 to π

            for (int j = 0; j <= columns; ++j)
            {
                float u = static_cast<float>(j) / columns;
                float longitude = u * 2.0f * M_PI; // 0 to 2π

                Vertex vertex;

                // Calculate position and normal
                if (i == 0)
                { // Bottom pole
                    vertex.position = kege::vec3(0.0f, -halfHeight - radius, 0.0f);
                    vertex.normal = kege::vec3(0.0f, -1.0f, 0.0f);
                    vertex.texcoord = kege::vec2(u, 0.0f);
                }
                else if (i == rings)
                { // Top pole
                    vertex.position = kege::vec3(0.0f, halfHeight + radius, 0.0f);
                    vertex.normal = kege::vec3(0.0f, 1.0f, 0.0f);
                    vertex.texcoord = kege::vec2(u, 1.0f);
                }
                else
                {
                    // Calculate spherical coordinates
                    float sinLat = std::sin(latitude);
                    float cosLat = std::cos(latitude);
                    float sinLong = std::sin(longitude);
                    float cosLong = std::cos(longitude);

                    vertex.normal = kege::vec3(cosLong * sinLat, cosLat, sinLong * sinLat);

                    // Determine if this is part of the bottom hemisphere, cylinder, or top hemisphere
                    if (latitude <= M_PI_2) { // Bottom hemisphere
                        vertex.position = vertex.normal * radius + kege::vec3(0.0f, -halfHeight, 0.0f);
                        vertex.texcoord = kege::vec2(u, v * 0.5f);
                    }
                    else { // Top hemisphere
                        vertex.position = vertex.normal * radius + kege::vec3(0.0f, halfHeight, 0.0f);
                        vertex.texcoord = kege::vec2(u, 0.5f + (v - 0.5f) * 0.5f);
                    }
                }

                vertices.push_back(vertex);
            }
        }

        // Generate cylindrical part if needed
        if (halfHeight > 0.0f)
        {
            for (int i = 1; i < rings; ++i)
            {
                float v = static_cast<float>(i) / rings;
                float y = -halfHeight + v * (2.0f * halfHeight);

                for (int j = 0; j <= columns; ++j)
                {
                    float u = static_cast<float>(j) / columns;
                    float angle = u * 2.0f * M_PI;

                    Vertex vertex;
                    vertex.position = kege::vec3(
                        radius * std::cos(angle),
                        y,
                        radius * std::sin(angle)
                    );
                    vertex.normal = kege::vec3(
                        std::cos(angle),
                        0.0f,
                        std::sin(angle)
                    );
                    vertex.texcoord = kege::vec2(u, 0.5f * (1.0f + y / (halfHeight + radius)));

                    vertices.push_back(vertex);
                }
            }
        }

        return vertices;
    }

    std::vector<unsigned int> generateCapsuleIndices(int columns, int rings)
    {
        std::vector<unsigned int> indices;

        // Generate indices for bottom hemisphere
        for (int i = 0; i < rings / 2; ++i) {
            for (int j = 0; j < columns; ++j) {
                int current = i * (columns + 1) + j;
                int next = current + columns + 1;

                indices.push_back(current);
                indices.push_back(next + 1);
                indices.push_back(next);

                indices.push_back(current);
                indices.push_back(current + 1);
                indices.push_back(next + 1);
            }
        }

        // Generate indices for cylindrical part (if any)
        if (rings > 2)
        {
            for (int i = rings / 2 - 1; i < rings / 2 + 1; ++i)
            {
                for (int j = 0; j < columns; ++j) {
                    int current = i * (columns + 1) + j;
                    int next = current + columns + 1;

                    indices.push_back(current);
                    indices.push_back(next + 1);
                    indices.push_back(next);

                    indices.push_back(current);
                    indices.push_back(current + 1);
                    indices.push_back(next + 1);
                }
            }
        }

        // Generate indices for top hemisphere
        for (int i = rings / 2; i < rings; ++i)
        {
            for (int j = 0; j < columns; ++j) {
                int current = i * (columns + 1) + j;
                int next = current + columns + 1;

                indices.push_back(current);
                indices.push_back(next + 1);
                indices.push_back(next);

                indices.push_back(current);
                indices.push_back(current + 1);
                indices.push_back(next + 1);
            }
        }

        return indices;
    }


    CapsuleMesh::CapsuleMesh( float height, float radius, int columns, int rings )
    {
        vertices = generateCapsuleVertices( height, radius, columns, rings );
        indices = generateCapsuleIndices( columns, rings );
    }
}
