//
//  mesh.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/22/24.
//

#include "mesh.hpp"

namespace kege{

    void computeTangentBitangent( std::vector< kege::Vertex >& vertices, const std::vector< uint32_t >& indices )
    {
        uint32_t triangle[3];
        for (int i = 0; i < indices.size(); i += 3 )
        {
            triangle[0] = indices[ i ];
            triangle[1] = indices[i+1];
            triangle[2] = indices[i+2];

            const kege::vec3& p0 = vertices[ triangle[0] ].position;
            const kege::vec3& p1 = vertices[ triangle[1] ].position;
            const kege::vec3& p2 = vertices[ triangle[2] ].position;

            const kege::vec2& uv0 = vertices[ triangle[0] ].texcoord;
            const kege::vec2& uv1 = vertices[ triangle[1] ].texcoord;
            const kege::vec2& uv2 = vertices[ triangle[2] ].texcoord;

            kege::vec3 edge1 = p1 - p0;
            kege::vec3 edge2 = p2 - p0;

            kege::vec2 deltaUV1 = uv1 - uv0;
            kege::vec2 deltaUV2 = uv2 - uv0;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            kege::vec3 tangent, bitangent;

            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            for (int i = 0; i < 3; ++i)
            {
                vertices[ triangle[i] ].tangent += tangent;
                vertices[ triangle[i] ].bitangent += bitangent;
            }
        }

        for (auto& vertex : vertices)
        {
            vertex.tangent = kege::normalize( vertex.tangent );
            vertex.bitangent = kege::normalize( vertex.bitangent );
        }
    }

}




namespace kege{

    const kege::IndirectDrawCommandBuffers& Mesh::getIndirectDrawCommandBuffers()const
    {
        return indirect_draw_buffers;
    }

    const kege::ShaderResourceBinder* Mesh::getShaderResourceBinder()const
    {
        return nullptr;
    }

    kege::ShaderResourceBinder* Mesh::getShaderResourceBinder()
    {
        return nullptr;
    }

    void Mesh::upload( kege::GraphicsDevice* graphics )
    {}

    void Mesh::unload( kege::GraphicsDevice* graphics )
    {}

    const kege::Buffer* Mesh::getVertexBuffer() const
    {
        return nullptr;
    }

    const kege::Buffer* Mesh::getIndexBuffer() const
    {
        return nullptr;
    }

    const kege::AABB& Mesh::getAABB() const
    {
        return aabb;
    }

    uint32_t Mesh::getTotalVertices() const
    {
        return total_vertices;
    }

    uint32_t Mesh::getTotalIndices() const
    {
        return total_indices;
    }


    Mesh::Mesh
    (
        const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
        const kege::AABB& aabb
    )
    :   indirect_draw_buffers( indirect_draw_buffers )
    ,   aabb( aabb )
    ,   total_vertices(0)
    ,   total_indices(0)
    {}

    Mesh::Mesh( const kege::AABB& aabb )
    :   aabb( aabb )
    ,   total_vertices(0)
    ,   total_indices(0)
    {}

    Mesh::Mesh()
    :   total_vertices(0)
    ,   total_indices(0)
    {}

    Mesh:: ~Mesh()
    {
        indirect_draw_buffers.clear();
    }
}


namespace kege{

    void StandardMesh::upload( kege::GraphicsDevice* graphics )
    {
        if ( !vertices.empty() )
        {
            total_indices = uint32_t( vertices.size() / sizeof( vertices[0] ) );
            vertex_buffer = graphics->createBuffer
            ({
                .size = sizeof(vertices[0]) * vertices.size(),
                .memory_usage = kege::MemoryUsage::GpuOnly,
                .usage = kege::BufferUsages::VertexBuffer,
                .data = vertices.data(),
                .name = "VertexBuffer",
                .frames = 1,
            });
        }

        if ( !indices.empty() )
        {
            total_indices = uint32_t( indices.size() / sizeof( indices[0] ) );
            index_buffer = graphics->createBuffer
            ({
                .size = sizeof( indices[0] ) * indices.size(),
                .memory_usage = kege::MemoryUsage::GpuOnly,
                .usage = kege::BufferUsages::IndexBuffer,
                .data = indices.data(),
                .frames = 1,
            });
        }
    }

    void StandardMesh::unload( kege::GraphicsDevice* graphics )
    {
        indirect_draw_buffers.clear();
        vertex_buffer.clear();
        index_buffer.clear();
    }

    void StandardMesh::bind( kege::CommandBuffer* cmd ) const
    {
        uint32_t first_binding = 0;
        std::vector<uint64_t> offsets = {0};
        std::vector<ref::Buffer> buffers = {vertex_buffer};
        cmd->bindVertexBuffers(first_binding, buffers, offsets);
    }

    const kege::Buffer* StandardMesh::getVertexBuffer() const
    {
        return vertex_buffer.ref();
    }

    const kege::Buffer* StandardMesh::getIndexBuffer() const
    {
        return index_buffer.ref();
    }

    std::vector< Vertex >& StandardMesh::getVertices()
    {
        return vertices;
    }

    std::vector< uint32_t >& StandardMesh::getIndices()
    {
        return indices;
    }

    StandardMesh::StandardMesh
    (
        const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
        const std::vector< Vertex >& vertices,
        const std::vector< uint32_t >& indices,
        const kege::AABB& aabb
    )
    :   kege::Mesh(indirect_draw_buffers, aabb)
    ,   vertices( std::move(vertices) )
    ,   indices( std::move(indices) )
    {}

    StandardMesh::StandardMesh
    (
        const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
        const std::vector< Vertex >& vertices,
        const kege::AABB& aabb
    )
    :   kege::Mesh(indirect_draw_buffers, aabb)
    ,   vertices( std::move(vertices) )
    {}

    StandardMesh::StandardMesh
    (
        const std::vector< Vertex >& vertices,
        const std::vector< uint32_t >& indices,
        const kege::AABB& aabb
    )
    :   kege::Mesh( aabb )
    ,   vertices( std::move(vertices) )
    ,   indices( std::move(indices) )
    {}

    StandardMesh::StandardMesh
    (
        const std::vector< Vertex >& vertices,
        const kege::AABB& aabb
    )
    :   kege::Mesh( aabb )
    ,   vertices( std::move(vertices) )
    {}

    StandardMesh::StandardMesh
    (
        const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
        const kege::ref::Buffer& vertex_buffer,
        const kege::ref::Buffer& index_buffer,
        const kege::AABB& aabb
    )
    :   kege::Mesh(indirect_draw_buffers, aabb)
    ,   vertex_buffer( vertex_buffer )
    ,   indices( index_buffer )
    {}

    StandardMesh::StandardMesh
    (
        const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
        const kege::ref::Buffer& vertex_buffer,
        const kege::AABB& aabb
    )
    :   kege::Mesh(indirect_draw_buffers, aabb)
    ,   vertex_buffer( vertex_buffer )
    {}

    StandardMesh::StandardMesh
    (
        const kege::ref::Buffer& vertex_buffer,
        const kege::ref::Buffer& index_buffer,
        const kege::AABB& aabb
    )
    :   kege::Mesh( aabb )
    ,   vertex_buffer( vertex_buffer )
    ,   indices( index_buffer )
    {}

    StandardMesh::StandardMesh
    (
        const kege::ref::Buffer& vertex_buffer,
        const kege::AABB& aabb
    )
    :   kege::Mesh( aabb )
    ,   vertex_buffer( vertex_buffer )
    {}

    StandardMesh::~StandardMesh()
    {
        indirect_draw_buffers.clear();
        vertex_buffer.clear();
        index_buffer.clear();
    }

    StandardMesh::StandardMesh()
    {}

}


namespace kege{

    const kege::ShaderResourceBinder* BufferMesh::getShaderResourceBinder()const
    {
        return binder.ref();
    }

    kege::ShaderResourceBinder* BufferMesh::getShaderResourceBinder()
    {
        return binder.ref();
    }

    void BufferMesh::bind( kege::CommandBuffer* cmd ) const
    {
        binder->bind( cmd );
    }

    BufferMesh::BufferMesh
    (
        const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
        const kege::ref::ShaderResourceBinder& binder,
        const kege::AABB& aabb
    )
    :   Mesh( indirect_draw_buffers, aabb )
    ,   binder( binder )
    {}

    BufferMesh::BufferMesh
    (
        const kege::ref::ShaderResourceBinder& binder,
        const kege::AABB& aabb
    )
    :   Mesh( aabb )
    ,   binder( binder )
    {}

    BufferMesh::~BufferMesh()
    {
        indirect_draw_buffers.clear();
        binder.clear();
    }

    BufferMesh::BufferMesh()
    {}

}


