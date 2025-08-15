//
//  landscape-system.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/28/24.
//

#include "landscape-system.hpp"

namespace kege{

    void LandscapeSystem::render( core::HierarchicalRenderList& render_objects )
    {}
    void LandscapeSystem::execute( kege::CommandBuffer& command_buffer )
    {
        ResourceSet* camera_shader_resource = getResourceManager()->get< ResourceSet >( "Camera" );
        RenderState* render_state = getScene()->getRenderState();

        command_buffer->setScissor( render_state->scissor );
        command_buffer->setViewport( render_state->viewport );
        command_buffer->bindResource( *camera_shader_resource );

        for ( Entity entity : *_entities )
        {
            entity.get< Ref< Landscape > >()->ref()->render( command_buffer );
        }
    }

    void LandscapeSystem::update( double dms )
    {
        if( !_entities || !getScene()->getCameraEntity() ) return;

        Transform* camera = getScene()->getCameraEntity().get< Transform >();

        for (Entity entity : *_entities )
        {
            Ref< Landscape >& landscape = *entity.get< Ref< Landscape > >();
            //Transform* transform = entity.get< Transform >();
            landscape->update( (dvec3)camera->position );
        }
    }

    bool LandscapeSystem::initialize()
    {
        _signature = createEntitySignature< Ref< Landscape >, Transform >();
        return System::initialize();
    }

    LandscapeSystem::LandscapeSystem( kege::Engine* engine )
    :   kege::System( "landscape", engine )
    {
    }
}

