//
//  system-factory.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/17/25.
//

#ifndef system_factory_hpp
#define system_factory_hpp

#include <functional>
#include <unordered_map>
#include "ref.hpp"

namespace kege{

    class System;
    class Engine;

    class SystemFactory
    {
    public:

        using CreateFunc = std::function< kege::Ref< kege::System >( kege::Engine* ) >;

        kege::Ref< kege::System > create( const std::string& name, kege::Engine* engine ) const;
        void registerSystem( const std::string& name, SystemFactory::CreateFunc func );
        static SystemFactory& instance();

    private:

        std::unordered_map< std::string, SystemFactory::CreateFunc > registry;
    };

}

#define KEGE_REGISTER_SYSTEM( CLASSNAME, NAME )                                 \
struct AutoRegister##CLASSNAME                                                  \
{                                                                               \
    static kege::Ref< kege::System > createSystem( kege::Engine* engine )       \
    {                                                                           \
        return kege::Ref< kege::System >( new kege::CLASSNAME( engine ) );   \
    }                                                                           \
    AutoRegister##CLASSNAME()                                                   \
    {                                                                           \
        SystemFactory::instance().registerSystem( NAME, createSystem );                 \
    }                                                                           \
};                                                                              \
static AutoRegister##CLASSNAME register_##CLASSNAME;                            \

#endif /* system_factory_hpp */
