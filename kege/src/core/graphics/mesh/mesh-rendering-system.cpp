//
//  mesh-rendering-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#include "../render/graph/render-executor.hpp"
#include "mesh-rendering-system.hpp"

namespace kege{

    void MeshRenderingSystem::render( double dms )
    {
        RenderObject object = {};
        for (auto [entity, geometry, transform] : view< kege::Geometry, kege::Transform >() )
        {
            //kege::Geometry* geometry = entity.get< kege::Geometry >();
            //kege::Transform* transform = entity.get< kege::Transform >();

            //for ( Ref< Mesh >& source : geometry->mesh->sources )
            {
                ref::Mesh& mesh = geometry->mesh;
                if ( geometry->object_transform.input_type == ShaderInput::PUSH_CONSTANT )
                {
                    if ( geometry->object_transform.transform == (Matrix::SCALE | Matrix::ROTATION | Matrix::TRANSLATION))
                    {
                        kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
                        m[1] = kege::quatToM44( transform->orientation );
                        m[0][0] = kege::vec4( m[1][0].xyz * transform->scale.x, 0.0 );
                        m[0][1] = kege::vec4( m[1][1].xyz * transform->scale.y, 0.0 );
                        m[0][2] = kege::vec4( m[1][2].xyz * transform->scale.z, 0.0 );
                        m[0][3] = kege::vec4( transform->position, 1.0 );
                    }
                    else if ( geometry->object_transform.transform == (Matrix::ROTATION | Matrix::TRANSLATION))
                    {
                        kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
                        m[1] = kege::quatToM44( transform->orientation );
                        m[0][0] = kege::vec4( m[1][0].xyz, 0.0 );
                        m[0][1] = kege::vec4( m[1][1].xyz, 0.0 );
                        m[0][2] = kege::vec4( m[1][2].xyz, 0.0 );
                        m[0][3] = kege::vec4( transform->position, 1.0 );
                    }
                    else if ( geometry->object_transform.transform == (Matrix::TRANSLATION))
                    {
                        kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
                        m[0][0] = kege::vec4( 1.0, 0.0, 0.0, 0.0 );
                        m[0][1] = kege::vec4( 0.0, 1.0, 0.0, 0.0 );
                        m[0][2] = kege::vec4( 0.0, 0.0, 1.0, 0.0 );
                        m[0][3] = kege::vec4( transform->position, 1.0 );
                    }
                    else if ( geometry->object_transform.transform == Matrix::ROTATION)
                    {
                        kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
                        m[0] = kege::quatToM44( transform->orientation );
                    }
                    object.constant.size = 2 * sizeof( kege::mat44 );
                    object.constant.stages = ShaderStageFlag::Vertex;
                }

//                if ( mesh->primative )
//                {
//                    if ( !mesh->primative->vertex_buffer )
//                    {
//                        mesh->primative->upload( getGraphics() );
//                    }
//                }
//
//                ref::Material material;
//                if ( geometry->material_set )
//                {
//                    if ( 0 <= mesh->material_index )
//                    {
//                        material = geometry->material_set->materials[ mesh->material_index ];
//                    }
//                    else
//                    {
//                        material = geometry->material_set->materials[ 0 ];
//                    }
//                }
//                else
//                {
//                    material = _default_material;
//                }
//
//                getRenderExecutor()->submit( material->getPass(), mesh, material.ref(), object.constant );
            }
        }
    }

//    bool MeshRenderingSystem::initialize()
//    {
//        Ref< Material >* material = getAssetManager()->fetch< Ref< Material > >( "default-material" );
//        if ( !material )
//        {
//            ShaderPipeline pipeline = getGraphics()->getShaderPipelineManager()->load
//            (
//                kege::vfs( "graphics-shaders/basic/basic.json" ).c_str()
//            );
//            _default_material = new Material({ new MaterialSource(RenderPassType::Geometry, pipeline, false, false, {}) });
//            getAssetManager()->add< Ref< Material > >( "default-material", _default_material );
//        }
//        else
//        {
//            _default_material = *material;
//        }
//        return EntitySystem::initialize();
//    }
//
//    void MeshRenderingSystem::shutdown()
//    {
//        EntitySystem::shutdown();
//    }

    MeshRenderingSystem::MeshRenderingSystem( kege::ECS* ecs )
    :   kege::ecs::System( ecs, "mesh-rendering-system", REQUIRE_RENDER  )
    {
//        _signature = createEntitySignature< kege::Geometry, kege::Transform >();
    }


    KEGE_REGISTER_ENTITY_SYSTEM( MeshRenderingSystem, "mesh-rendering" );
}
