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

    void InstanceDrawBuffer::setBuffers( int binding_index, const kege::BufferBindings& bindings, int frame )
    {
        _shader_set.set->bind( frame, binding_index, bindings );
    }

    void InstanceDrawBuffer::setImages( int binding_index, const kege::ImageBindings& bindings, int frame )
    {
        _shader_set.set->bind( frame, binding_index, bindings );
    }

    const kege::BindSet& InstanceDrawBuffer::getShaderSet() const
    {
        return _shader_set;
    }

    InstanceDrawBuffer::InstanceDrawBuffer( IndexedSetLayout layout )
    {
        _shader_set.set = layout.set->allocateSet();
        _shader_set.index = layout.index;
    }

}



namespace kege{
//    void func()
//    {
//        InstanceDrawBuffer77* mesh = 0;
//        mesh->setInstanceBufferBindings("", {});
//        mesh->setInstanceBufferBinding(0, 0, kege::BufferBindings{});
//
//        mesh->setInstanceBuffer();
//    }

    
//    const ref::ShaderSet& InstanceBufferList::getShaderBindings( int index )const
//    {
//        return buffers[ index ].getShaderBindings();
//    }
//
//    const ref::Buffer& InstanceBufferList::getBufferHandle( int index )const
//    {
//        return buffers[ index ].getBufferHandle();
//    }
//
//    InstanceBufferList::InstanceBufferList( const std::vector< InstanceBuffer >& buffers )
//    :   buffers( buffers )
//    {}
//
//    InstanceBufferList::~InstanceBufferList()
//    {
//        buffers.clear();
//    }
//
//    InstanceBufferList::InstanceBufferList()
//    {}
//
//
//
//    IndirectDrawBufferList::IndirectDrawBufferList
//    (
//        const std::vector< IndirectDrawBuffer >& buffers
//    )
//    :   buffers( buffers )
//    {}
//
//    IndirectDrawBufferList::~IndirectDrawBufferList()
//    {
////        if ( graphics )
////        {
////            for ( IndirectDrawBuffer& buffer : buffers )
////            {
////                graphics->destroyBuffer( buffer.buffer );
////            }
////            buffers.clear();
////            graphics = nullptr;
////        }
//    }
//    IndirectDrawBufferList::IndirectDrawBufferList()
//    {}




//    void MeshPrimitive::upload( Graphics* graphics )
//    {}
//
//    void MeshPrimitive::unload( Graphics* graphics )
//    {}
//    
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
        vertices.clear();
        indices.clear();
    }

    MeshPrimitive::MeshPrimitive()
    :   drawcount(0)
    {}

    void MeshPrimitive::unload( kege::Graphics* graphics )
    {
        if ( vertex_buffer ) vertex_buffer.clear();
        if ( index_buffer ) index_buffer->clear();
        //this->graphics = graphics;
    }

    void MeshPrimitive::upload( kege::Graphics* graphics )
    {
        vertex_buffer = graphics->createBuffer
        ({
            sizeof( kege::Vertex ) * vertices.size(), vertices.data(),
            BufferUsages::VertexBuffer,
            MemoryUsage::GpuOnly
        });

        if ( !indices.empty() )
        {
            index_buffer = graphics->createBuffer
            ({
                sizeof( indices[0] ) * indices.size(), indices.data(),
                BufferUsages::IndexBuffer,
                MemoryUsage::GpuOnly
            });
        }
    }





//    void Mesh::setIndirectDrawCommandBuffer( int index, const IndirectDrawCommandBuffer& info )
//    {
//        if ( !indirect_draw_object )
//        {
//            indirect_draw_object = new kege::IndirectDrawObject({ info });
//        }
//        else
//        {
//            indirect_draw_object->at( index ) = info;
//        }
//    }
//
//    void Mesh::setIndirectDrawCommandBuffers( const kege::IndirectDrawObject& info )
//    {
//        indirect_draw_object = new kege::IndirectDrawObject( info );
//    }
//
//    void Mesh::resizeIndirectDrawCommandBuffers( size_t size )
//    {
//        if ( !indirect_draw_object )
//        {
//            indirect_draw_object = new kege::IndirectDrawObject( size );
//            return;
//        }
//        indirect_draw_object->resize( size );
//    }

    const ref::IndirectDrawObject& Mesh::getIndirectDrawObject()const
    {
        return indirect_draw_object;
    }

    const ref::ShaderData& Mesh::getInstanceShaderData()const
    {
        return shader_data;
    }

    ref::IndirectDrawObject Mesh::getIndirectDrawObject()
    {
        return indirect_draw_object;
    }

    ref::ShaderData Mesh::getInstanceShaderData()
    {
        return shader_data;
    }

    void Mesh::upload( Graphics* graphics )
    {
        if (primative)
        {
            primative->upload( graphics );
        }
    }

    void Mesh::unload( Graphics* graphics )
    {
        if (primative)
        {
            primative->unload( graphics );
        }
    }

    Mesh::Mesh
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

    Mesh::Mesh
    (
        ref::MeshPrimitive primative,
        ref::IndirectDrawObject indirect_draw_object,
        ref::ShaderData instance_draw_buffer
    )
    :   primative( primative )
    ,   indirect_draw_object( indirect_draw_object )
    ,   shader_data( instance_draw_buffer )
    ,   instance_count( 0 )
    ,   first_instance( 0 )
    ,   first_index( 0 )
    ,   index_count( 0 )
    ,   material_index( -1 )
    {}

    Mesh::Mesh
    (
        ref::MeshPrimitive primative,
        ref::IndirectDrawObject indirect_draw_object
    )
    :   primative( primative )
    ,   indirect_draw_object( indirect_draw_object )
    ,   instance_count( 0 )
    ,   first_instance( 0 )
    ,   first_index( 0 )
    ,   index_count( 0 )
    ,   material_index( -1 )
    {}

    Mesh::Mesh
    (
        ref::MeshPrimitive primative,
        ref::ShaderData instance_draw_buffer
    )
    :   primative( primative )
    ,   shader_data( instance_draw_buffer )
    ,   instance_count( 0 )
    ,   first_instance( 0 )
    ,   first_index( 0 )
    ,   index_count( 0 )
    ,   material_index( -1 )
    {}

    Mesh::Mesh()
    :   primative()
    ,   instance_count( 0 )
    ,   first_instance( 0 )
    ,   first_index( 0 )
    ,   index_count( 0 )
    ,   material_index( -1 )
    {}

}

