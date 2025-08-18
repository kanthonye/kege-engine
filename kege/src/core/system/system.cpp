//
//  system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#include "system.hpp"

namespace kege{

    bool System::initialize()
    {
        return true;
    }

    void System::shutdown()
    {
    }

    const std::string& System::getName() const
    {
        return _name;
    }

    System::System( kege::Engine* engine, const std::string& name )
    :   _engine( engine )
    ,   _name( name )
    {
    }

    System:: ~System()
    {
    }


}




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
