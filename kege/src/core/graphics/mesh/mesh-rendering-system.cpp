//
//  mesh-rendering-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#include "../render/graph/render-executor.hpp"
#include "../../io/virtual-directory.hpp"
#include "mesh-rendering-system.hpp"

namespace kege{

    void MeshRenderingSystem::render( double dms )
    {
        kege::AssetManager* assets = getScene()->getAssetManager();
        ref::RenderExecutor executor = getScene()->getRenderGraph()->getRenderExecutor();

        RenderObject object = {};
        for (auto [entity, renderable, transform] : view< kege::Renderable, kege::Transform >() )
        {
            kege::Ref<Mesh>* meshptr = assets->get<kege::Ref<Mesh>>( renderable->mesh_handle );
            if ( meshptr == nullptr)
            {
                continue;
            }
            kege::Ref<Mesh>& mesh = *meshptr;

            
            //for ( Ref< Mesh >& source : geometry->mesh->sources )
            {

//                if ( geometry->object_transform.input_type == ShaderInput::PUSH_CONSTANT )
//                {
//                    if ( geometry->object_transform.transform == (Matrix::SCALE | Matrix::ROTATION | Matrix::TRANSLATION))
//                    {
//                        kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
//                        m[1] = kege::quatToM44( transform->orientation );
//                        m[0][0] = kege::vec4( m[1][0].xyz * transform->scale.x, 0.0 );
//                        m[0][1] = kege::vec4( m[1][1].xyz * transform->scale.y, 0.0 );
//                        m[0][2] = kege::vec4( m[1][2].xyz * transform->scale.z, 0.0 );
//                        m[0][3] = kege::vec4( transform->position, 1.0 );
//                    }
//                    else if ( geometry->object_transform.transform == (Matrix::ROTATION | Matrix::TRANSLATION))
//                    {
//                        kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
//                        m[1] = kege::quatToM44( transform->orientation );
//                        m[0][0] = kege::vec4( m[1][0].xyz, 0.0 );
//                        m[0][1] = kege::vec4( m[1][1].xyz, 0.0 );
//                        m[0][2] = kege::vec4( m[1][2].xyz, 0.0 );
//                        m[0][3] = kege::vec4( transform->position, 1.0 );
//                    }
//                    else if ( geometry->object_transform.transform == (Matrix::TRANSLATION))
//                    {
//                        kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
//                        m[0][0] = kege::vec4( 1.0, 0.0, 0.0, 0.0 );
//                        m[0][1] = kege::vec4( 0.0, 1.0, 0.0, 0.0 );
//                        m[0][2] = kege::vec4( 0.0, 0.0, 1.0, 0.0 );
//                        m[0][3] = kege::vec4( transform->position, 1.0 );
//                    }
//                    else if ( geometry->object_transform.transform == Matrix::ROTATION)
//                    {
//                        kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
//                        m[0] = kege::quatToM44( transform->orientation );
//                    }
//                    object.constant.size = 2 * sizeof( kege::mat44 );
//                    object.constant.stages = ShaderStageFlag::Vertex;
//                }

//                if ( mesh->primative )
//                {
//                    if ( !mesh->primative->vertex_buffer )
//                    {
//                        mesh->primative->upload( getGraphics() );
//                    }
//                }
//
                kege::mat44* m = reinterpret_cast< kege::mat44* >( object.constant.data );
                m[1] = kege::quatToM44( transform->orientation );
                m[0][0] = kege::vec4( m[1][0].xyz * transform->scale.x, 0.0 );
                m[0][1] = kege::vec4( m[1][1].xyz * transform->scale.y, 0.0 );
                m[0][2] = kege::vec4( m[1][2].xyz * transform->scale.z, 0.0 );
                m[0][3] = kege::vec4( transform->position, 1.0 );
                object.constant.size = 2 * sizeof( kege::mat44 );
                object.constant.stages = ShaderStageFlag::Vertex;

                ref::Material material;
                if ( mesh->material_index <= 0 )
                {
                    Ref< Material >* material_ptr = assets->fetch< Ref< Material > >( "default" );
                    if ( !material_ptr )
                    {
                        uint64_t pipeline_id = assets->load< ref::ShaderPipeline >
                        (
                            kege::vfs( "graphics-shaders/basic/basic.json" ).c_str()
                        );

                        ref::ShaderPipeline* pipeline = assets->get<ref::ShaderPipeline>(pipeline_id);
                        material = new Material(kege::RenderPassType::Geometry, *pipeline, {});
                        assets->add< ref::Material >( "default", material );
                    }
                    else
                    {
                        material = *material_ptr;
                    }
                }
                else
                {
                    material = *assets->get<ref::Material>( mesh->material_index );
                }

                executor->submit( material->getPass(), mesh, material.ref(), object.constant );
            }
        }
    }

    MeshRenderingSystem::MeshRenderingSystem( kege::ECS* ecs )
    :   kege::ecs::System( ecs, "mesh-rendering-system", REQUIRE_RENDER  )
    {
    }


    KEGE_REGISTER_ENTITY_SYSTEM( MeshRenderingSystem, "mesh-rendering" );
}
