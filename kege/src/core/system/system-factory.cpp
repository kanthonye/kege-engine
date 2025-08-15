//
//  system-factory.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/17/25.
//

#include "../../../src/core/system/system.hpp"
#include "system-factory.hpp"

namespace kege{

    void SystemFactory::registerSystem( const std::string& name, SystemFactory::CreateFunc func )
    {
        registry[ name ] = std::move( func );
    }

    kege::Ref< kege::System > SystemFactory::create( const std::string& name, kege::Engine* engine ) const
    {
        auto it = registry.find(name);
        if ( it != registry.end() )
        {
            return it->second( engine );
        }
        return nullptr;
    }

    SystemFactory& SystemFactory::instance()
    {
        static SystemFactory factory;
        return factory;
    }

}
