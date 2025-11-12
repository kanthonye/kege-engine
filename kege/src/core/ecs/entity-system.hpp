//
//  entity-system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#ifndef entity_system_hpp
#define entity_system_hpp

#include "../ecs/entity.hpp"
#include "../ecs/entity-registry.hpp"
#include "../ecs/entity-logic.hpp"
#include "../ecs/system.hpp"

namespace kege{

    class EntitySystemManager;

    class EntitySystem : public kege::RefCounter
    {
    public:

        enum StateBitFlag
        {
            REQUIRE_UPDATE = 1,
            REQUIRE_RENDER = 2,
            REQUIRE_INPUT  = 4,
        };

        EntitySystem( const std::string& n, uint32_t r, kege::EntitySystemManager* m );

        virtual void update( double dms );
        virtual void render( double dms );
        virtual void input( double dms );

        virtual bool initialize();
        virtual void shutdown();

        const kege::EntitySignature& getEntitySignature()const;
        const std::string& getName() const;

        kege::Scene* getScene();
        void onSceneChanged();

        kege::RenderExecutor* getRenderExecutor();
        kege::AssetManager* getAssetManager();
        kege::MappedInputs& getMappedInputs();
        kege::Graphics* getGraphics();
        kege::EntityView* getEntities();

        bool checkFlag( StateBitFlag flag );

        virtual ~EntitySystem();

    protected:

        kege::EntitySystemManager* _manager;
        kege::EntitySignature _signature;
        kege::EntityView* _entities;
    
        uint32_t _requirements;
        std::string _name;

        friend kege::EntitySystemManager;
    };

}

namespace kege{

    class EntitySystemFactory
    {
    public:

        using CreateFunc = std::function< kege::Ref< kege::EntitySystem >( kege::EntitySystemManager* ) >;

        kege::Ref< kege::EntitySystem > create( const std::string& name, kege::EntitySystemManager* m ) const;
        void registerSystem( const std::string& name, EntitySystemFactory::CreateFunc func );
        static EntitySystemFactory& instance();

    private:

        std::unordered_map< std::string, EntitySystemFactory::CreateFunc > registry;
    };
}

#define KEGE_REGISTER_ENTITY_SYSTEM( CLASSNAME, NAME )                          \
struct EcsAutoRegister##CLASSNAME                                               \
{                                                                               \
    static kege::Ref< kege::EntitySystem > createSystem( kege::EntitySystemManager* m )  \
    {                                                                            \
        return kege::Ref< kege::EntitySystem >( new kege::CLASSNAME( m ) ); \
    }                                                                           \
    EcsAutoRegister##CLASSNAME()                                                \
    {                                                                           \
        EntitySystemFactory::instance().registerSystem( NAME, createSystem );         \
    }                                                                           \
};                                                                              \
static EcsAutoRegister##CLASSNAME ecs_register_##CLASSNAME;                     \


#endif /* entity_system_hpp */
