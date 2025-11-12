//
//  mesh.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/22/24.
//

#ifndef mesh_hpp
#define mesh_hpp

#include "../../utils/array.hpp"
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

    struct IndirectDrawCommandBuffer
    {
        ref::Buffer buffer;
        uint64_t offset;
        uint32_t count;
        uint32_t stride;
    };

    struct IndirectDrawObject : public kege::array< kege::IndirectDrawCommandBuffer >
    {
        IndirectDrawObject( const std::initializer_list< kege::IndirectDrawCommandBuffer >& init )
        :   kege::array< kege::IndirectDrawCommandBuffer >( init )
        {}
    };
}
namespace kege::ref{
    using IndirectDrawObject = kege::Ref< kege::IndirectDrawObject >;
}
namespace kege::cref{
    using IndirectDrawObject = kege::Ref< const kege::IndirectDrawObject >;
}





namespace kege{

    struct InstanceDrawBuffer : public kege::RefCounter
    {
    public:

        /** 
         * @brief Sets buffer bindings for a specific binding index.
         *
         * @param binding_index Index of the binding within the set.
         * @param bindings Vector of buffer bindings to set.
         * @param frame Frame index for double/triple buffering scenarios.
         */
        void setBuffers( int binding_index, const kege::BufferBindings& bindings, int frame = 0 );

        /** 
         * @brief Sets image bindings for a specific binding index.
         *
         * @param binding_index Index of the binding within the set.
         * @param bindings Vector of image bindings to set.
         * @param frame Frame index for double/triple buffering scenarios.
         */
        void setImages( int binding_index, const kege::ImageBindings& bindings, int frame = 0 );

        /** 
         * @brief Retrieves the associated shader set.
         * @return Handle to the shader set.
         */
        const kege::IndexedSet& getShaderSet() const;

        /** 
         * @brief Constructor for InstanceDrawBuffer.
         * @param layout Indexed set layout defining the bindings.
         */
        InstanceDrawBuffer( IndexedSetLayout layout );

    private:

        kege::IndexedSet _shader_set;
    };

}
namespace kege::ref{
    using InstanceDrawBuffer = kege::Ref< kege::InstanceDrawBuffer >;
}
namespace kege::cref{
    using InstanceDrawBuffer = kege::Ref< const kege::InstanceDrawBuffer >;
}





namespace kege{

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

        // CPU vertex buffer data
        std::vector< Vertex > vertices;

        // CPU index buffer data
        std::vector< uint32_t > indices;

        // GPU vertex buffer handle
        ref::Buffer vertex_buffer;

        // GPU index buffer handle
        ref::Buffer index_buffer;

        kege::vec3 aabb_min;
        kege::vec3 aabb_max;
        
        uint32_t drawcount;
    };
}

namespace kege::ref{
    typedef kege::Ref< kege::MeshPrimitive > MeshPrimitive;
}
namespace kege::sref{
    typedef kege::Ref< const kege::MeshPrimitive > MeshPrimitive;
}


namespace kege{

    enum class PrimitiveType { Mesh, ScreenSpaceQuad, PointList };

    class Mesh : public kege::RefCounter
    {
    public:

        const ref::IndirectDrawObject& getIndirectDrawObject()const;
        const ref::ShaderData& getInstanceShaderData()const;

        ref::IndirectDrawObject getIndirectDrawObject();
        ref::ShaderData getInstanceShaderData();

        void upload( Graphics* graphics );
        void unload( Graphics* graphics );

        Mesh
        (
            ref::MeshPrimitive primative,
            uint32_t instance_count,
            uint32_t first_instance,
            uint32_t first_index,
            uint32_t index_count,
            int32_t  material_index = 1
        );

        Mesh
        (
            ref::MeshPrimitive primative,
            ref::IndirectDrawObject indirect_draw_object,
            ref::ShaderData instance_draw_object
        );

        Mesh
        (
            ref::MeshPrimitive primative,
            ref::IndirectDrawObject indirect_draw_object
        );

        Mesh
        (
            ref::MeshPrimitive primative,
            ref::ShaderData instance_draw_object
        );

        Mesh
        ();

        ref::IndirectDrawObject indirect_draw_object;
        ref::ShaderData shader_data;
        ref::MeshPrimitive primative;

        PrimitiveType primitive_type;
        uint32_t instance_count = 0;
        uint32_t first_instance = 0;
        uint32_t first_index = 0;
        uint32_t index_count = 0;

        int material_index = -1;
    };
    typedef kege::Ref< kege::Mesh > MeshSourceRef;

}
namespace kege::ref{
    typedef kege::Ref< kege::Mesh > Mesh;
}
namespace kege::sref{
    typedef kege::Ref< const kege::Mesh > Mesh;
}


namespace kege{
    using MeshSet = kege::array< ref::Mesh >;
}
namespace kege::ref{
    using MeshSet = kege::Ref< kege::MeshSet >;
}
namespace kege::sref{
    using MeshSet = kege::Ref< const kege::MeshSet >;
}



namespace kege{

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
        Ref< MaterialSet > material_set;
        Procedure object_transform;
    };

    void computeTangentBitangent( std::vector< kege::Vertex >& vertices, const std::vector< uint32_t >& indices );
};

#endif /* mesh_hpp */
