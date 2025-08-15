//
//  update-light-shader-resource.cpp
//  kege
//
//  Created by Kenneth Esdaile on 9/26/24.
//

#include "update-light-shader-resource.hpp"
namespace kege{

    void UpdateLightShaderResource::render( core::HierarchicalRenderList& render_objects )
    {
//        GlobalLights* lights = getResourceManager()->get< GlobalLights >( "Lights" );
//        ShaderResourceLayout* shader_resource = getResourceManager()->get< ShaderResourceLayout >( "Lights" );
//        if ( shader_resource )
//        {
//            lights->count = 0;
//            
//            for ( kege::Entity& entity : *_entities )
//            {
//                kege::Transform* transform = entity.get< kege::Transform >();
//                Light* light = entity.get< Light >();
//
//                if ( lights->count >= MAX_LIGHTS )
//                {
//                    break;
//                }
//
//                light->position = transform->position;
//                lights->data[ lights->count ] = *light;
//                lights->count += 1;
//            }
//
//            kege::Buffer buffer = shader_resource->ref()->getShaderResources()[0].buffer;
//            buffer->copyFrom( 0, sizeof( uint32_t ), &lights->count );
//            buffer->copyFrom( sizeof( vec4 ), sizeof( uint32_t ), &lights->count );
//        }
    }

    bool UpdateLightShaderResource::initialize()
    {
//        getResourceManager()->insert< GlobalLights >( "Lights", {} );
//
//        GlobalLights* lights = getResourceManager()->get< GlobalLights >( "Lights" );
//        lights->data.resize( MAX_LIGHTS );
//        lights->count = 0;
//
//        const size_t size = MAX_LIGHTS * sizeof( Light ) + sizeof( vec4 );
//        ShaderResourceLayout shader_resource = ShaderResourceLayout::create
//        (
//            {{{ 0 , "LightBlock", 1, graphicsDESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, graphicsFRAGMENT_SHADER, }}},
//            {{ ShaderResource( kege::Buffer::createUniformBuffer( DYNAMIC_BUFFER, size, nullptr ) ) }}
//        );
//        getResourceManager()->insert< ShaderResourceLayout >( "Lights", shader_resource );
//
//        _signature = createEntitySignature< Light, Transform >();
        return System::initialize();
    }

    void UpdateLightShaderResource::shutdown()
    {
//        getResourceManager()->remove< ShaderResourceLayout >( "Lights" );
    }

    UpdateLightShaderResource::UpdateLightShaderResource( kege::Engine* engine )
    :   kege::System( "update-light-resource-set", engine )
    {}
}
