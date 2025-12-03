//
//  ecs-system-factory.cpp
//  assets
//
//  Created by Kenneth Esdaile on 11/28/25.
//

#include "ecs-system-factory.hpp"


namespace kege::ecs{

    void EntitySystemFactory::registerSystem( const std::string& name, EntitySystemFactory::CreateFunc func )
    {
        registry[ name ] = std::move( func );
    }

    kege::Ref< kege::ecs::System > EntitySystemFactory::create( const std::string& name, kege::ECS* m ) const
    {
        auto it = registry.find(name);
        if ( it != registry.end() )
        {
            return it->second( m );
        }
        return nullptr;
    }

    EntitySystemFactory& EntitySystemFactory::instance()
    {
        static EntitySystemFactory factory;
        return factory;
    }

}
