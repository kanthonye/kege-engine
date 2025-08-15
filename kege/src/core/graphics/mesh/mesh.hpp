//
//  mesh.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/22/24.
//

#ifndef mesh_hpp
#define mesh_hpp

#include "vectors.hpp"
#include "material.hpp"
#include "quaternion.hpp"
#include "quaternion.hpp"
#include "transform.hpp"

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

    struct Primitive : public kege::RefCounter
    {
        enum{ VERTEX_DRAW, INDEX_DRAW, VERTEX_DRAW_COMMANDS, INDEX_DRAW_COMMANDS};
        virtual void init( kege::Graphics* graphics ){};
        virtual void draw( CommandEncoder* encoder ) = 0;
        virtual ~Primitive(){}

        uint32_t material_index;
    };

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

    struct IndexPrimitive : public kege::Primitive
    {
        void init( kege::Graphics* graphics ){};
        void draw( CommandEncoder* encoder );
        IndexDrawCommand draw_index_command;
    };

    struct VertexPrimitive : public kege::Primitive
    {
        void init( kege::Graphics* graphics ){};
        void draw( CommandEncoder* encoder );
        VertexDrawCommand draw_vertex_command;
    };

    struct IndexPrimitives : public kege::Primitive
    {
        void init( kege::Graphics* graphics );
        void draw( CommandEncoder* encoder );
        std::vector< IndexDrawCommand > index_draw_commands;
        kege::BufferHandle indirect_draw_buffer;
        uint64_t offset;
        uint32_t count;
        uint32_t stride;
    };

    struct VertexPrimitives : public kege::Primitive
    {
        void init( kege::Graphics* graphics );
        void draw( CommandEncoder* encoder );
        std::vector< VertexDrawCommand > vertex_draw_commands;
        kege::BufferHandle indirect_draw_buffer;
        uint64_t offset;
        uint32_t count;
        uint32_t stride;
    };



    struct Geometry : public kege::RefCounter
    {
        //virtual void draw( CommandEncoder* encoder ) = 0;
        //virtual void bind() = 0;
        virtual ~Geometry(){}
    };

    struct Mesh : public kege::Geometry
    {
        void init( kege::Graphics* graphics );

        std::vector< kege::Ref< Primitive > > primatives;
        std::vector< Vertex > vertices;
        std::vector< uint32_t > indices;

        kege::BufferHandle vertex_buffer;
        kege::BufferHandle index_buffer;
    };

    void computeTangentBitangent( std::vector< kege::Vertex >& vertices, const std::vector< uint32_t >& indices );
    void uploadMesh( kege::Graphics* graphics, kege::Mesh& mesh );
};

#endif /* mesh_hpp */
