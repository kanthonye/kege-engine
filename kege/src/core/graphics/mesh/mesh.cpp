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


    InstanceBufferList::InstanceBufferList
    (
        kege::Graphics* graphics,
        const std::vector< InstanceBuffer >& buffers
    )
    :   graphics( graphics )
    ,   buffers( buffers )
    {}

    InstanceBufferList::~InstanceBufferList()
    {
        if ( graphics )
        {
            for ( InstanceBuffer& instance : buffers )
            {
                graphics->destroyBuffer( instance.buffer );
                graphics->freeShaderResource( 1, &instance.shader_resource );
            }
            buffers.clear();
            graphics = nullptr;
        }
    }
    InstanceBufferList::InstanceBufferList()
    :   graphics( nullptr )
    {}



    IndirectDrawBufferList::IndirectDrawBufferList
    (
        kege::Graphics* graphics,
        const std::vector< IndirectDrawBuffer >& buffers
    )
    :   graphics( graphics )
    ,   buffers( buffers )
    {}

    IndirectDrawBufferList::~IndirectDrawBufferList()
    {
        if ( graphics )
        {
            for ( IndirectDrawBuffer& buffer : buffers )
            {
                graphics->destroyBuffer( buffer.buffer );
            }
            buffers.clear();
            graphics = nullptr;
        }
    }
    IndirectDrawBufferList::IndirectDrawBufferList()
    :   graphics( nullptr )
    {}



    MeshPrimitive::MeshPrimitive
    (
        const std::vector< Vertex >& vertices,
        const std::vector< uint32_t >& indices,
        const kege::vec3& aabb_min,
        const kege::vec3& aabb_max
    )
    :   vertices( vertices )
    ,   indices( indices )
    ,   aabb_max( aabb_max )
    ,   aabb_min( aabb_min )
    ,   drawcount( uint32_t( !indices.empty() ? indices.size() : vertices.size() ) )
    {
    }

    MeshPrimitive::~MeshPrimitive()
    {
        if ( this->graphics )
        {
            unload( this->graphics );
        }
        vertices.clear();
        indices.clear();
    }

    MeshPrimitive::MeshPrimitive()
    :   drawcount(0)
    {}

    void MeshPrimitive::unload( kege::Graphics* graphics )
    {
        if ( vertex_buffer ) graphics->destroyBuffer( vertex_buffer );
        if ( index_buffer ) graphics->destroyBuffer( index_buffer );
        this->graphics = graphics;
    }

    void MeshPrimitive::upload()
    {
        vertex_buffer = graphics->createBuffer
        ({
            sizeof( kege::Vertex ) * vertices.size(),
            vertices.data(),
            BufferUsage::VertexBuffer,
            MemoryUsage::GpuOnly
        });

        if ( !indices.empty() )
        {
            index_buffer = graphics->createBuffer
            ({
                sizeof( uint32_t ) * indices.size(),
                indices.data(),
                BufferUsage::IndexBuffer,
                MemoryUsage::GpuOnly
            });
        }
    }



    MeshSource::MeshSource
    (
        Ref< MeshPrimitive > primative,
        uint32_t instance_count,
        uint32_t first_instance,
        uint32_t first_index,
        uint32_t index_count,
        int32_t material_index
    )
    :   primative( primative )
    ,   instance_count( instance_count )
    ,   first_instance( first_instance )
    ,   first_index( first_index )
    ,   index_count( index_count )
    ,   material_index( material_index )
    {}

    MeshSource::MeshSource()
    :   primative()
    ,   instance_count( 0 )
    ,   first_instance( 0 )
    ,   first_index( 0 )
    ,   index_count( 0 )
    ,   material_index( -1 )
    {}



    Mesh::Mesh( const std::vector< Ref< MeshSource > >& sources )
    :   sources( sources )
    {}
    Mesh::Mesh()
    {}

}

