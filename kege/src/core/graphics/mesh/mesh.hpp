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
        const ShaderBindings& getShaderBindings()const;
        const BufferHandle& getBufferHandle()const;

        kege::Ref< ShaderResrc > resource;
        uint32_t instance_count;
        uint32_t first_instance;
    };
    class InstanceBufferList : public kege::RefCounter
    {
    public:

        InstanceBufferList( const std::vector< InstanceBuffer >& buffers );

        const ShaderBindings& getShaderBindings( int index )const;
        const BufferHandle& getBufferHandle( int index )const;

        ~InstanceBufferList();
        InstanceBufferList();

        std::vector< InstanceBuffer > buffers;
    };

    struct IndirectDrawBuffer
    {
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
            const std::vector< IndirectDrawBuffer >& buffers
        );

        ~IndirectDrawBufferList();
        IndirectDrawBufferList();

        std::vector< IndirectDrawBuffer > buffers;
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
        void upload( kege::Graphics* graphics );

        std::vector< Vertex > vertices;
        std::vector< uint32_t > indices;

        // GPU vertex buffer handle
        kege::BufferHandle vertex_buffer;

        // GPU index buffer handle
        kege::BufferHandle index_buffer;

        kege::vec3 aabb_min;
        kege::vec3 aabb_max;
        
        uint32_t drawcount;
    };
    typedef kege::Ref< kege::MeshPrimitive > MeshPrimitiveRef;

    enum class PrimitiveType { Mesh, ScreenSpaceQuad, PointList };

    struct MeshSource : public kege::RefCounter
    {
        void upload( Graphics* graphics );
        void unload( Graphics* graphics );

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
        (
            Ref< MeshPrimitive > primative,
            Ref< IndirectDrawBufferList > indirect_draw_buffer_list,
            Ref< InstanceBufferList > instance_buffer_list
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
    typedef kege::Ref< kege::MeshSource > MeshSourceRef;

    struct Mesh : public kege::RefCounter
    {
        Mesh( const std::vector< kege::MeshSourceRef >& sources );
        Mesh();
        
        std::vector< kege::MeshSourceRef > sources;
        kege::vec3 aabb_min;
        kege::vec3 aabb_max;
    };
    typedef kege::Ref< kege::Mesh > MeshRef;




    enum struct Matrix : char
    {
        SCALE           = 1 << 0,
        ROTATION        = 1 << 1,
        TRANSLATION     = 1 << 2,
        PERSPECTIVE     = 1 << 3,
        ORTHOGRAPHIC    = 1 << 4
    };

    inline constexpr Matrix operator|(Matrix a, Matrix b)
    {
        return static_cast< Matrix >(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline constexpr Matrix& operator|=(Matrix& a, Matrix b)
    {
        return a = a | b;
    }

    inline constexpr Matrix operator&(Matrix a, Matrix b)
    {
        return static_cast< Matrix >(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline constexpr Matrix& operator&=(Matrix& a, Matrix b)
    {
        return a = a & b;
    }
    enum struct ShaderInput : char
    {
        NONE,
        BUFFER,
        PUSH_CONSTANT,
    };

    struct Procedure
    {
        ShaderInput input_type = ShaderInput::PUSH_CONSTANT;
        Matrix transform = Matrix::SCALE | Matrix::ROTATION | Matrix::TRANSLATION;
        Matrix rotation = Matrix::ROTATION;
    };

    struct Geometry
    {
        Ref< Mesh > mesh;
        Ref< Material > material;
        Procedure object_transform;
    };

    void computeTangentBitangent( std::vector< kege::Vertex >& vertices, const std::vector< uint32_t >& indices );
};

#endif /* mesh_hpp */
