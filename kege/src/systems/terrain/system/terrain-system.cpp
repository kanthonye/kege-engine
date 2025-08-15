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
        ShaderPipeline* pipeline = ShaderPipelineLibrary::get( "spherical-terrain-shader" );
        RenderState* render_state = getScene()->getRenderState();

        /**
         * Important. must always set the viewport and scissor area before rendering
         */
        command_buffer->setViewport( render_state->viewport );
        command_buffer->setScissor( render_state->scissor );

        command_buffer->bindShaderPipeline( *pipeline );
        command_buffer->bindShaderResource( *pipeline, global_resources->shader_resource_camera );

        TerrainTransformPushConstant terrain_block;
        Terrain* terrain;
        for ( Entity entity : *_entities )
        {
            terrain = entity.get< component::Terrain >()->ref();
            Transform* transform = entity.get< Transform >();

            terrain_block = *transform;
            terrain_block.data.spherical_radius = terrain->radius();
            command_buffer->pushConstants( *pipeline, terrain_block.push_constant );

            terrain->draw( command_buffer );
        }
    }

    void TerrainSystem::update( double dms )
    {
        if( !_entities ) return;
        if ( !getScene()->getCameraEntity() ) return;

        Transform* camera = getScene()->getCameraEntity().get< Transform >();

        for (Entity entity : *_entities )
        {
            component::Terrain* terrain = entity.get< component::Terrain >();
            Transform* transform = entity.get< Transform >();

            terrain->ref()->setCameraPosition( camera->position );
            terrain->ref()->setRotation( transform->orientation );
            terrain->ref()->setPosition( transform->position );
            terrain->ref()->update();
        }
        prepareGeometries();
    }

    bool TerrainSystem::initialize()
    {
        temp_render_buffer.resize(1024*1024);
        _entities = getEntities< component::Terrain, Transform >();
        return System::initialize();
    }

    void TerrainSystem::prepareGeometries()
    {
        if( !_entities ) return;

        component::Terrain* terrain;

        for (Entity entity : *_entities )
        {
            terrain = entity.get< component::Terrain >();
            terrain->ref()->prepareGeometries( temp_render_buffer );
        }
    }

    TerrainSystem::TerrainSystem(): System< component::Terrain, Transform >(0);
}
