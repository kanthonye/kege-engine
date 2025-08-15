//
//  component-factory.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/7/25.
//

#include "component-factory.hpp"

namespace kege{

    void ComponentFactory::registerComponent(const std::string& name, ComponentFactory::Func func)
    {
        component_registry[name] = func;
    }

    bool ComponentFactory::addComponent(const std::string& name, const Json& data, kege::Entity& entity )
    {
        auto i = component_registry.find( name );
        if (i != component_registry.end())
        {
            if( i->second( data, entity ) )
            {
                return true;
            }
        }
        return false;
    }

    ComponentFactory& ComponentFactory::instance()
    {
        static ComponentFactory instance;
        return instance;
    }

}
