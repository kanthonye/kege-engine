//
//  mesh-rendering-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/18/25.
//

#include "mesh-rendering-system.hpp"

namespace kege{

    void MeshRenderingSystem::render( double dms )
    {
        RenderObject object = {};
        for (kege::Entity entity : *_entities )
        {
            kege::Geometry* geometry = entity.get< kege::Geometry >();
            kege::Transform* transform = entity.get< kege::Transform >();

            for ( Ref< MeshSource >& source : geometry->mesh->sources )
            {
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
                    object.constant.stages = ShaderStage::Vertex;
                }

                if ( source->primative )
                {
                    if ( !source->primative->vertex_buffer )
                    {
                        source->primative->upload( _engine->graphics().get() );
                    }
                }

                if ( 0 <= source->material_index && geometry->material )
                {
                    object.material = geometry->material->sources[ source->material_index ];
                }
                else
                {
                    object.material = _default_material->sources[0];
                }

                object.mesh = source;

                _engine->renderManager()->submit( object );
            }
        }
    }

    bool MeshRenderingSystem::initialize()
    {
        Ref< Material >* material = _engine->getAssetManager().fetch< Ref< Material > >( "default-material" );
        if ( !material )
        {
            ShaderPipeline pipeline = _engine->graphics()->getShaderPipelineManager()->load
            (
                kege::vfs( "graphics-shaders/basic/basic.json" ).c_str()
            );

            _default_material = new Material({ new MaterialSource(RenderPassType::Geometry, pipeline, false, false, {}) });
            _engine->getAssetManager().add< Ref< Material > >( "default-material", _default_material );
        }
        else
        {
            _default_material = *material;
        }
        return EntitySystem::initialize();
    }

    void MeshRenderingSystem::shutdown()
    {
        EntitySystem::shutdown();
    }

    MeshRenderingSystem::MeshRenderingSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "mesh-rendering-system", REQUIRE_RENDER )
    {
        _signature = createEntitySignature< kege::Geometry, kege::Transform >();
    }


    KEGE_REGISTER_ENTITY_SYSTEM( MeshRenderingSystem, "mesh-rendering" );
}
