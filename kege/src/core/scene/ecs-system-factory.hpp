//
//  ecs-system-factory.hpp
//  assets
//
//  Created by Kenneth Esdaile on 11/28/25.
//

#ifndef ecs_system_factory_hpp
#define ecs_system_factory_hpp

#include "ecs-system.hpp"

namespace kege::ecs{

    class EntitySystemFactory
    {
    public:

        using CreateFunc = std::function< kege::Ref< kege::ecs::System >( kege::ECS* ) >;

        kege::Ref< kege::ecs::System > create( const std::string& name, kege::ECS* m ) const;
        void registerSystem( const std::string& name, EntitySystemFactory::CreateFunc func );
        static EntitySystemFactory& instance();

    private:

        std::unordered_map< std::string, EntitySystemFactory::CreateFunc > registry;
    };
}

#define KEGE_REGISTER_ENTITY_SYSTEM( CLASSNAME, NAME )                          \
struct EcsAutoRegister##CLASSNAME                                               \
{                                                                               \
    static kege::Ref< kege::ecs::System > createSystem( kege::ECS* m )  \
    {                                                                            \
        return kege::Ref< kege::ecs::System >( new kege::CLASSNAME( m ) ); \
    }                                                                           \
    EcsAutoRegister##CLASSNAME()                                                \
    {                                                                           \
        ecs::EntitySystemFactory::instance().registerSystem( NAME, createSystem );         \
    }                                                                           \
};                                                                              \
static EcsAutoRegister##CLASSNAME ecs_register_##CLASSNAME;                     \

#endif /* ecs_system_factory_hpp */
