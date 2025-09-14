//
//  mesh.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/22/24.
//

#ifndef mesh_hpp
#define mesh_hpp

#include "../../math/algebra/vectors.hpp"
#include "../../math/algebra/quaternion.hpp"
#include "../../math/algebra/transform.hpp"
#include "../material/material.hpp"

namespace kege{

    struct Vertex
    {
        kege::fvec3 position;
        kege::fvec3 normal;
        kege::fvec2 texcoord;
        kege::fvec3 tangent;
        kege::fvec3 bitangent;
    };

    struct SkinVertex
    {
        kege::fvec3 position;
        kege::fvec3 normal;
        kege::fvec2 texcoord;
        kege::fvec3 tangent;
        kege::fvec3 bitangent;
        kege::fvec4 weights;
        kege::fvec4 joints;
    };

    struct ModelMatrices
    {
        void operator()( const kege::vec3& position, const kege::quat& orientation, const kege::vec3& scale )
        {
            rotation = kege::quatToM44( orientation );
            transform[ 0 ] = kege::vec4( rotation[ 0 ].xyz * scale.x, 0.0 );
            transform[ 1 ] = kege::vec4( rotation[ 1 ].xyz * scale.y, 0.0 );
            transform[ 2 ] = kege::vec4( rotation[ 2 ].xyz * scale.z, 0.0 );
            transform[ 3 ] = kege::vec4( position, 1.0 );
        }
        ModelMatrices( const kege::vec3& position, const kege::quat& qrotation, const kege::vec3& scale )
        {
            operator()( position, qrotation, scale );
        }
        ModelMatrices( const kege::Transform& transform )
        {
            operator()( transform.position, transform.orientation, transform.scale );
        }
        ModelMatrices()
        {}

        kege::mat44 transform;
        kege::mat44 rotation;
    };

}



namespace kege{

    struct IndexDrawCommand
    {
        uint32_t index_count;
        uint32_t instance_count;
        uint32_t first_index;
        int32_t  vertex_offset;
        uint32_t first_instance;
    };

    struct VertexDrawCommand
    {
        uint32_t vertex_count;
        uint32_t instance_count;
        uint32_t first_vertex;
        uint32_t first_instance;
    };





    struct InstanceBuffer
    {
        kege::ShaderResource shader_resource;
        kege::BufferHandle buffer;
        uint32_t instance_count;
        uint32_t first_instance;
    };
    class InstanceBufferList : public kege::RefCounter
    {
    public:

        InstanceBufferList
        (
            kege::Graphics* graphics,
            const std::vector< InstanceBuffer >& buffers
        );

        ~InstanceBufferList();
        InstanceBufferList();

        std::vector< InstanceBuffer > buffers;
        kege::Graphics* graphics;
    };



    
    struct IndirectDrawBuffer
    {
        kege::ShaderResource shader_resource;
        kege::BufferHandle buffer;
        uint64_t offset;
        uint32_t count;
        uint32_t stride;
    };

    class IndirectDrawBufferList : public kege::RefCounter
    {
    public:

        IndirectDrawBufferList
        (
            kege::Graphics* graphics,
            const std::vector< IndirectDrawBuffer >& buffers
        );

        ~IndirectDrawBufferList();
        IndirectDrawBufferList();

        std::vector< IndirectDrawBuffer > buffers;
        kege::Graphics* graphics;
    };




    class MeshPrimitive : public kege::RefCounter
    {
    public:
        
        MeshPrimitive
        (
            const std::vector< Vertex >& vertices,
            const std::vector< uint32_t >& indices,
            const kege::vec3& aabb_min,
            const kege::vec3& aabb_max
        );
        ~MeshPrimitive();
        MeshPrimitive();

        void unload( kege::Graphics* graphics );
        void upload();

        std::vector< Vertex > vertices;
        std::vector< uint32_t > indices;

        // GPU vertex buffer handle
        kege::BufferHandle vertex_buffer;

        // GPU index buffer handle
        kege::BufferHandle index_buffer;

        kege::vec3 aabb_min;
        kege::vec3 aabb_max;
        
        kege::Graphics* graphics;
        uint32_t drawcount;
    };

    enum class PrimitiveType { Mesh, ScreenSpaceQuad, PointList };

    struct MeshSource : public kege::RefCounter
    {
        MeshSource
        (
            Ref< MeshPrimitive > primative,
            uint32_t instance_count,
            uint32_t first_instance,
            uint32_t first_index,
            uint32_t index_count,
            int32_t material_index = 1
        );
        
        MeshSource
        ();

        Ref< IndirectDrawBufferList > indirect_draw_buffer_list;
        Ref< InstanceBufferList > instance_buffer_list;
        Ref< MeshPrimitive > primative;

        PrimitiveType primitive_type;
        uint32_t instance_count = 0;
        uint32_t first_instance = 0;
        uint32_t first_index = 0;
        uint32_t index_count = 0;

        int material_index = -1;
    };

    struct Mesh : public kege::RefCounter
    {
        Mesh( const std::vector< Ref< MeshSource > >& sources );
        Mesh();
        
        std::vector< Ref< MeshSource > > sources;
        kege::vec3 aabb_min;
        kege::vec3 aabb_max;
    };

    struct Geometry
    {
        Ref< Mesh > mesh;
        Ref< Material > material;
    };

    void computeTangentBitangent( std::vector< kege::Vertex >& vertices, const std::vector< uint32_t >& indices );
};

#endif /* mesh_hpp */
