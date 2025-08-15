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






    void VertexPrimitives::init( kege::Graphics* graphics )
    {
        count = static_cast< int >( vertex_draw_commands.size() );
        stride = sizeof( vertex_draw_commands[0] );
        offset = 0;

        indirect_draw_buffer = graphics->createBuffer
        ({
             .size = stride * vertex_draw_commands.size(),
             .data = vertex_draw_commands.data(),
             .memory_usage = MemoryUsage::CpuToGpu,
             .usage = BufferUsage::IndirectBuffer
        });
    }
    void VertexPrimitives::draw( CommandEncoder* encoder )
    {
        encoder->drawIndirect( indirect_draw_buffer, offset, count, stride );
    }

    void IndexPrimitives::init( kege::Graphics* graphics )
    {
        count = static_cast< int >( index_draw_commands.size() );
        stride = sizeof( index_draw_commands[0] );
        offset = 0;

        indirect_draw_buffer = graphics->createBuffer
        ({
             .size = stride * index_draw_commands.size(),
             .data = index_draw_commands.data(),
             .memory_usage = MemoryUsage::CpuToGpu,
             .usage = BufferUsage::IndirectBuffer
        });
    }
    void IndexPrimitives::draw( CommandEncoder* encoder )
    {
        encoder->drawIndexIndirect( indirect_draw_buffer, offset, count, stride );
    }

    void IndexPrimitive::draw( CommandEncoder* encoder )
    {
        encoder->drawIndexed
        (
            draw_index_command.index_count,
            draw_index_command.instance_count,
            draw_index_command.first_index,
            draw_index_command.vertex_offset,
            draw_index_command.first_instance
        );
    }

    void VertexPrimitive::draw( CommandEncoder* encoder )
    {
        encoder->draw
        (
            draw_vertex_command.vertex_count,
            draw_vertex_command.instance_count,
            draw_vertex_command.first_vertex,
            draw_vertex_command.first_instance
        );
    }


    void Mesh::init( kege::Graphics* graphics )
    {
        vertex_buffer = graphics->createBuffer
        ({
            sizeof( kege::Vertex ) * vertices.size(),
            vertices.data(),
            BufferUsage::VertexBuffer,
            MemoryUsage::GpuOnly
        });

        index_buffer = graphics->createBuffer
        ({
            sizeof( uint32_t ) * indices.size(),
            indices.data(),
            BufferUsage::IndexBuffer,
            MemoryUsage::GpuOnly
        });

        if ( primatives.empty() )
        {
            if ( index_buffer )
            {
                IndexPrimitive* primitve = new IndexPrimitive;
                primitve->draw_index_command.first_index = 0;
                primitve->draw_index_command.index_count = static_cast< int >( indices.size() );
                primitve->draw_index_command.first_instance = 0;
                primitve->draw_index_command.instance_count = 1;
                primitve->draw_index_command.vertex_offset = 0;
                primatives.push_back( primitve );
            }
            else if ( index_buffer )
            {
                VertexPrimitive* primitve = new VertexPrimitive;
                primitve->draw_vertex_command.first_vertex = 0;
                primitve->draw_vertex_command.vertex_count = static_cast< int >( vertices.size() );
                primitve->draw_vertex_command.first_instance = 0;
                primitve->draw_vertex_command.instance_count = 1;
                primatives.push_back( primitve );
            }
        }
        else
        {
            for(kege::Ref< Primitive >& primative : primatives)
            {
                primative->init( graphics );
            }
        }
    }

}

