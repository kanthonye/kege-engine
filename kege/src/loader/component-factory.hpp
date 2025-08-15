//
//  component-factory.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 2/7/25.
//

#ifndef component_factory_hpp
#define component_factory_hpp

#include <functional>
#include <unordered_map>
#include "entity.hpp"
#include "json-parser.hpp"

namespace kege{

    class ComponentFactory
    {
    public:

        using Func = std::function< bool( const Json&, kege::Entity& ) >;

        bool addComponent(const std::string& name, const Json& data, kege::Entity& entity );
        void registerComponent(const std::string& name, ComponentFactory::Func func);
        static ComponentFactory& instance();

    private:

        std::unordered_map< std::string, ComponentFactory::Func > component_registry;
    };
}

#endif /* component_factory_hpp */
