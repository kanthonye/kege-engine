//
//  entity-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#ifndef entity_system_hpp
#define entity_system_hpp

#include "../../core/engine/engine.hpp"
#include "../../core/ecs/entity.hpp"
#include "../../core/ecs/entity-registry.hpp"
#include "../../core/system/system.hpp"

namespace kege{

    class EntitySystemManager;

    class EntitySystem : public kege::System
    {
    public:

        enum StateBitFlag
        {
            REQUIRE_UPDATE = 1,
            REQUIRE_RENDER = 2,
            REQUIRE_INPUT  = 4,
        };

        EntitySystem( kege::Engine* engine, const std::string& name, uint32_t requirements );

        virtual void input( const std::vector< Input >& inputs );
        virtual void update( double dms );
        virtual void render( double dms );

        virtual void onSceneChange();
        virtual bool initialize()override;
        virtual void shutdown()override;

        bool checkFlag( StateBitFlag flag );
        EntityView* getEntities(){return _entities;}

        virtual ~EntitySystem();

    protected:

        kege::EntitySignature _signature;
        EntityView* _entities;
        uint32_t _requirements;

        friend kege::EntitySystemManager;
    };

}

namespace kege{

    class EntitySystemFactory
    {
    public:

        using CreateFunc = std::function< kege::Ref< kege::EntitySystem >( kege::Engine* ) >;

        kege::Ref< kege::EntitySystem > create( const std::string& name, kege::Engine* engine ) const;
        void registerSystem( const std::string& name, EntitySystemFactory::CreateFunc func );
        static EntitySystemFactory& instance();

    private:

        std::unordered_map< std::string, EntitySystemFactory::CreateFunc > registry;
    };
}

#define KEGE_REGISTER_ENTITY_SYSTEM( CLASSNAME, NAME )                          \
struct EcsAutoRegister##CLASSNAME                                               \
{                                                                               \
    static kege::Ref< kege::EntitySystem > createSystem( kege::Engine* engine )  \
    {                                                                            \
        return kege::Ref< kege::EntitySystem >( new kege::CLASSNAME( engine ) ); \
    }                                                                           \
    EcsAutoRegister##CLASSNAME()                                                \
    {                                                                           \
        EntitySystemFactory::instance().registerSystem( NAME, createSystem );         \
    }                                                                           \
};                                                                              \
static EcsAutoRegister##CLASSNAME ecs_register_##CLASSNAME;                     \


#endif /* entity_system_hpp */
