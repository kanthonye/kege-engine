//
//  mesh.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/22/24.
//

#ifndef mesh_hpp
#define mesh_hpp

#include "../../../utils/array.hpp"
#include "../../../math/algebra/vectors.hpp"
#include "../../../math/algebra/quaternion.hpp"
#include "../../../math/algebra/transform.hpp"
#include "../../../math/geometry/primitive-3D-shapes.hpp"
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

    using IndirectDrawCommandBuffers = kege::array< kege::IndirectDrawCommandBuffer >;
}


namespace kege{

    enum class PrimitiveType { Mesh, ScreenSpaceQuad, PointList };

    class Mesh : public kege::RefCounter
    {
    public:

        const kege::IndirectDrawCommandBuffers& getIndirectDrawCommandBuffers()const;
        virtual const kege::ShaderResourceBinder* getShaderResourceBinder()const;
        virtual kege::ShaderResourceBinder* getShaderResourceBinder();

        virtual void upload( kege::GraphicsDevice* graphics );
        virtual void unload( kege::GraphicsDevice* graphics );
        virtual void bind( kege::CommandBuffer* cmd ) const = 0;

        virtual const kege::Buffer* getVertexBuffer() const;
        virtual const kege::Buffer* getIndexBuffer() const;

        const kege::AABB& getAABB() const;
        uint32_t getTotalVertices() const;
        uint32_t getTotalIndices() const;

        Mesh
        (
            const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
            const kege::AABB& aabb
        );
        Mesh( const kege::AABB& aabb );
        Mesh();

        virtual ~Mesh();

        kege::IndirectDrawCommandBuffers indirect_draw_buffers;
        kege::AABB aabb;

        uint32_t total_indices;
        uint32_t total_vertices;
    };

}



namespace kege{

    struct StandardMesh : public kege::Mesh
    {
        void upload( kege::GraphicsDevice* graphics );
        void unload( kege::GraphicsDevice* graphics );
        void bind( kege::CommandBuffer* cmd ) const;

        const kege::Buffer* getVertexBuffer() const;
        const kege::Buffer* getIndexBuffer() const;

        // CPU vertex buffer data
        std::vector< Vertex >& getVertices();

        // CPU index buffer data
        std::vector< uint32_t >& getIndices();

        StandardMesh
        (
            const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
            const std::vector< Vertex >& vertices,
            const std::vector< uint32_t >& indices,
            const kege::AABB& aabb
        );
        StandardMesh
        (
            const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
            const std::vector< Vertex >& vertices,
            const kege::AABB& aabb
        );
        StandardMesh
        (
            const std::vector< Vertex >& vertices,
            const std::vector< uint32_t >& indices,
            const kege::AABB& aabb
        );
        StandardMesh
        (
            const std::vector< Vertex >& vertices,
            const kege::AABB& aabb
        );
        StandardMesh
        (
            const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
            const kege::ref::Buffer& vertex_buffer,
            const kege::ref::Buffer& index_buffer,
            const kege::AABB& aabb
        );
        StandardMesh
        (
            const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
            const kege::ref::Buffer& vertex_buffer,
            const kege::AABB& aabb
        );
        StandardMesh
        (
            const kege::ref::Buffer& vertex_buffer,
            const kege::ref::Buffer& index_buffer,
            const kege::AABB& aabb
        );
        StandardMesh
        (
            const kege::ref::Buffer& vertex_buffer,
            const kege::AABB& aabb
        );
        StandardMesh();

        virtual ~StandardMesh();

    protected:

        // CPU vertex buffer data
        std::vector< Vertex > vertices;

        // CPU index buffer data
        std::vector< uint32_t > indices;

        // GPU vertex buffer handle
        ref::Buffer vertex_buffer;

        // GPU index buffer handle
        ref::Buffer index_buffer;
    };

}


namespace kege{

    struct BufferMesh : public kege::Mesh
    {
        const kege::ShaderResourceBinder* getShaderResourceBinder()const;
        kege::ShaderResourceBinder* getShaderResourceBinder();
        void bind( kege::CommandBuffer* cmd ) const;

        BufferMesh
        (
            const kege::IndirectDrawCommandBuffers& indirect_draw_buffers,
            const kege::ref::ShaderResourceBinder& binder,
            const kege::AABB& aabb
        );
        BufferMesh
        (
            const kege::ref::ShaderResourceBinder& binder,
            const kege::AABB& aabb
        );
        BufferMesh();

        virtual ~BufferMesh();

    protected:

        kege::ref::ShaderResourceBinder binder;
    };

}






namespace kege{

    struct Renderable
    {
        kege::Sphere sphere;
        uint64_t mesh_id = 0; // Handle to either StandardMesh or BufferMesh
        uint64_t material_id = 0;  // Handle to material/shader
        uint32_t instance_count = 0; // Instances for this command
        uint32_t first_instance = 0; // Starting instance ID
        uint32_t start_index = 0; // First index to draw
        uint32_t draw_count = 0; // Number of indices (0 = use all)
        uint32_t vertex_offset = 0;
        uint32_t render_layer = 0;
        
        kege::RenderPassType pass;
        bool culled = false;            // Frustum/occlusion culling result
    };

//    struct Renderable
//    {
//        FeatureMask feature;
//
//        uint64_t mesh_handle     = 0;
//        uint64_t material_handle = 0;
//        uint64_t pipeline_handle = 0;
//
//        uint32_t render_layer = 0;
//        uint32_t render_pass = 0;
//
//        bool is_visible         = true;
//        bool is_static          = true;
//
//        char fname[64] = {0};
//        size_t size = 0;
//    };
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



namespace kege{

    struct PushConstantBlock
    {
        float data[64];
        int offset;
        int size;
        ShaderStageFlag stages = ShaderStageFlag::AllGraphics;
    };

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

//    struct Geometry
//    {
//        Ref< Mesh > mesh;
//        Ref< MaterialSet > material_set;
//        Procedure object_transform;
//    };

    void computeTangentBitangent( std::vector< kege::Vertex >& vertices, const std::vector< uint32_t >& indices );

}



namespace kege{

    enum struct Feature : uint64_t
    {
        UNDEFINED               = 0,
        VERTEX_POSITION         = 1ull << 0,
        VERTEX_NORMAL           = 1ull << 1,
        VERTEX_TEXCOORD         = 1ull << 2,
        VERTEX_TANGENT          = 1ull << 3,
        VERTEX_BITANGENT        = 1ull << 4,
        VERTEX_COLOR            = 1ull << 5,

        /**
         * @include -> vertex joints
         * @include -> vertex weights
         * @include -> skeletal transform buffer
         */
        VERTEX_SKINNING         = 1ull << 6,

        /**
         * @include -> material uniform
         */
        MATERIAL                = 1ull << 7,

        /**
         * @include -> noraml mapping texture
         */
        NORMAL_MAPPING          = 1ull << 8,

        /**
         * @integrate -> parallax mapping calculation
         * @requires -> vertex tangent and bitangent
         */
        PARALLAX_MAPPING        = 1ull << 9,

        /**
         * @include -> lighting function
         */
        LIT                     = 1ull << 10,

        /**
         * @integrate -> point lights
         */
        LIGHT_POINT             = 1ull << 11,

        /**
         * @integrate -> spot lights
         */
        LIGHT_SPOT              = 1ull << 12,
        LIGHT_AREA              = 1ull << 13,

        /**
         * @integrate -> image base lighting calculation
         */
        IIMAGE_BASE_LIGHTING    = 1ull << 14,
        INSTANCE_RENDERING      = 1ull << 15,
        ANISOTROPIC             = 1ull << 16,
        CAST_SHADOW             = 1ull << 17,
        RECIEVE_SHADOW          = 1ull << 18,
    };

    typedef Feature FeatureMask;

    inline constexpr FeatureMask operator|(const Feature& a, const Feature& b)
    {
        return static_cast<FeatureMask>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
    }

    inline constexpr FeatureMask operator&(Feature a, Feature b)
    {
        return static_cast<FeatureMask>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b));
    }

    inline constexpr FeatureMask& operator|=(Feature& a, Feature b)
    {
        a = a | b;
        return a;
    }

    inline constexpr FeatureMask& operator&=(Feature& a, Feature b)
    {
        a = a & b;
        return a;
    }

    inline constexpr bool contain(const FeatureMask& a, const Feature& b)
    {
        return (a & b) == b;
    }

};

#endif /* mesh_hpp */
