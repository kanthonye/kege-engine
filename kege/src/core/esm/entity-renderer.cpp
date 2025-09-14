//
//  entity-renderer.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#include "entity-renderer.hpp"
namespace kege{

    EntityRenderer::EntityRenderer( kege::InitContext context )
    :   kege::EntityLogic( context.engine, context.name )
    ,   _graphics( context.graphics )
    ,   _graph( context.graph )
    {}

    EntityRenderer:: ~EntityRenderer()
    {}

}

namespace kege{

    void EntityRendererFactory::registerRenderer( const std::string& name, EntityRendererFactory::CreateFunc func )
    {
        registry[ name ] = std::move( func );
    }

    kege::Ref< kege::EntityRenderer > EntityRendererFactory::create
    (
        const std::string& name,
        const kege::InitContext& context
    )
    const
    {
        auto it = registry.find(name);
        if ( it != registry.end() )
        {
            return it->second( context );
        }
        return nullptr;
    }

    EntityRendererFactory& EntityRendererFactory::instance()
    {
        static EntityRendererFactory factory;
        return factory;
    }

}
