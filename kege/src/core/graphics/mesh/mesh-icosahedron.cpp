//
//  mesh-icosahedron.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#include "mesh-icosahedron.hpp"
namespace kege{

    IcosahedronMesh::IcosahedronMesh( float radius )
    {
        generateIcosahedronMesh( radius, vertices, indices );
    }

    void generateIcosahedronMesh( float radius, std::vector<Vertex>& out_vertices, std::vector<uint32_t>& out_indices )
    {
        out_vertices.clear();
        out_indices.clear();

        const float phi = (1.0f + std::sqrt(5.0f)) * 0.5f; // golden ratio
        const float invLen = 1.0f / std::sqrt(1 + phi * phi);

        // 12 vertices of a unit icosahedron
        kege::vec3 positions[] = {
            {-1,  phi, 0}, { 1,  phi, 0}, {-1, -phi, 0}, { 1, -phi, 0},
            {0, -1,  phi}, {0,  1,  phi}, {0, -1, -phi}, {0,  1, -phi},
            { phi, 0, -1}, { phi, 0,  1}, {-phi, 0, -1}, {-phi, 0,  1}
        };

        for (auto& p : positions)
            p = kege::normalize(p); // make unit length

        // 20 triangular faces
        uint32_t indices[] = {
            0, 11, 5,  0, 5, 1,   0, 1, 7,    0, 7, 10,  0, 10, 11,
            1, 5, 9,   5, 11, 4, 11, 10, 2,  10, 7, 6,   7, 1, 8,
            3, 9, 4,   3, 4, 2,   3, 2, 6,    3, 6, 8,   3, 8, 9,
            4, 9, 5,   2, 4, 11,  6, 2, 10,   8, 6, 7,   9, 8, 1
        };

        out_vertices.resize(12);
        for (size_t i = 0; i < 12; ++i)
        {
            // convert spherical coords to UVs
            float u = 0.5f + std::atan2( positions[i].z, positions[i].x ) / (2.0f * float( _PI_ ));
            float vcoord = 0.5f - std::asin( positions[i].y ) / float( _PI_ );

            Vertex v;
            v.position = positions[i] * radius;
            v.normal = positions[i];
            v.texcoord = kege::vec2(u, vcoord);

            v.tangent = kege::vec3(0); // placeholder, will compute below
            out_vertices[i] = v;
        }

        out_indices.assign(indices, indices + sizeof(indices) / sizeof(uint32_t));

        // Compute tangents using triangle geometry
        std::vector<kege::vec3> tan1(out_vertices.size(), kege::vec3(0.0f));
        for (size_t i = 0; i < out_indices.size(); i += 3)
        {
            uint32_t i0 = out_indices[i + 0];
            uint32_t i1 = out_indices[i + 1];
            uint32_t i2 = out_indices[i + 2];

            Vertex& v0 = out_vertices[i0];
            Vertex& v1 = out_vertices[i1];
            Vertex& v2 = out_vertices[i2];

            kege::vec3 edge1 = v1.position - v0.position;
            kege::vec3 edge2 = v2.position - v0.position;

            kege::vec2 deltaUV1 = v1.texcoord - v0.texcoord;
            kege::vec2 deltaUV2 = v2.texcoord - v0.texcoord;

            float f = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
            if (f == 0.0f) continue; // skip degenerate
            float invF = 1.0f / f;

            kege::vec3 tangent = invF * (deltaUV2.y * edge1 - deltaUV1.y * edge2);

            tan1[i0] += tangent;
            tan1[i1] += tangent;
            tan1[i2] += tangent;
        }

        // Normalize tangents
        for (size_t i = 0; i < out_vertices.size(); ++i)
        {
            kege::vec3& n = out_vertices[i].normal;
            kege::vec3& t = tan1[i];

            // Gram-Schmidt orthogonalize
            out_vertices[i].tangent = kege::normalize(t - n * kege::dot(n, t));
        }
    }
}
