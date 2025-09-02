//
//  terrain.cpp
//  new-engine
//
//  Created by Kenneth Esdaile on 3/4/24.
//

#include "terrain-system.hpp"

namespace kege{

    void TerrainSystem::execute( kege::CommandBuffer* command_buffer )
    {
//        ShaderPipeline* pipeline = ShaderPipelineLibrary::get( "spherical-terrain-shader" );
//        RenderState* render_state = getScene()->getRenderState();
//
//        /**
//         * Important. must always set the viewport and scissor area before rendering
//         */
//        command_buffer->setViewport( render_state->viewport );
//        command_buffer->setScissor( render_state->scissor );
//
//        command_buffer->bindShaderPipeline( *pipeline );
//        command_buffer->bindShaderResource( *pipeline, global_resources->shader_resource_camera );
//
//        TerrainTransformPushConstant terrain_block;
//        Terrain* terrain;
//        for ( Entity entity : *_entities )
//        {
//            terrain = entity.get< component::Terrain >()->ref();
//            Transform* transform = entity.get< Transform >();
//
//            terrain_block = *transform;
//            terrain_block.data.spherical_radius = terrain->radius();
//            command_buffer->pushConstants( *pipeline, terrain_block.push_constant );
//
//            terrain->draw( command_buffer );
//        }
    }

    void TerrainSystem::update( double dms )
    {
        if( !_entities ) return;

        vec3 camera_position;
        if ( _engine->scene()->getCameraEntity() )
        {
            camera_position = _engine->scene()->getCameraEntity().get< Transform >()->position;
        }
        else
        {
            camera_position = vec3( 0.f );
        }

        for (Entity entity : *_entities )
        {
            Terrain* terrain = entity.get< Terrain >();
            Transform* transform = entity.get< Transform >();

            //terrain->setOrientation( transform->orientation );
            //terrain->setPosition( transform->position );
            terrain->update( camera_position );
        }
    }

    bool TerrainSystem::initialize()
    {
        _signature = createEntitySignature< Terrain, Transform >();
        return EntitySystem::initialize();
    }

//    void TerrainSystem::prepareGeometries()
//    {
//        if( !_entities ) return;
//
//        component::Terrain* terrain;
//
//        for (Entity entity : *_entities )
//        {
//            terrain = entity.get< component::Terrain >();
//            terrain->ref()->prepareGeometries( temp_render_buffer );
//        }
//    }

    TerrainSystem::~TerrainSystem()
    {}
    TerrainSystem::TerrainSystem( kege::Engine* engine )
    :   kege::EntitySystem( engine, "terrain-system", REQUIRE_UPDATE | REQUIRE_RENDER )
    {}
}
