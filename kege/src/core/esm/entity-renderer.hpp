//
//  entity-renderer.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#ifndef entity_renderer_hpp
#define entity_renderer_hpp

#include "../engine/engine.hpp"
#include "../ecs/entity.hpp"
#include "../ecs/entity-registry.hpp"
#include "../ecs/entity-logic.hpp"
#include "../system/system.hpp"

namespace kege{

    class EntityRendererManager;

    struct InitContext
    {
        std::string name;
        kege::Engine* engine;
        kege::Graphics* graphics;
        kege::RenderGraph* graph;
    };

    class EntityRenderer : public kege::EntityLogic
    {
    public:

        explicit EntityRenderer( kege::InitContext context );

        virtual void render( double dms ) = 0;
        virtual bool initialize() = 0;
        virtual void shutdown() = 0;
        virtual void clear() = 0;

        virtual ~EntityRenderer();

    protected:

        kege::Graphics* _graphics;
        kege::RenderGraph* _graph;
    };

}

namespace kege{

    class EntityRendererFactory
    {
    public:

        using CreateFunc = std::function< kege::Ref< kege::EntityRenderer >( const kege::InitContext& ) >;

        kege::Ref< kege::EntityRenderer > create( const std::string& name, const kege::InitContext& context ) const;
        void registerRenderer( const std::string& name, EntityRendererFactory::CreateFunc func );
        static EntityRendererFactory& instance();

    private:

        std::unordered_map< std::string, EntityRendererFactory::CreateFunc > registry;
    };
}

#define KEGE_REGISTER_ENTITY_RENDER( CLASSNAME, NAME )                          \
struct AutoRegisterEntityRenderer##CLASSNAME                                               \
{                                                                               \
    static kege::Ref< kege::EntityRenderer > create( const kege::InitContext& context )  \
    {                                                                            \
        return kege::Ref< kege::EntityRenderer >( new kege::CLASSNAME( engine ) ); \
    }                                                                           \
    AutoRegisterEntityRenderer##CLASSNAME()                                                \
    {                                                                           \
        EntityRendererFactory::instance().registerSystem( NAME, create );         \
    }                                                                           \
};                                                                              \
static AutoRegisterEntityRenderer##CLASSNAME ecs_register_##CLASSNAME;                     \


#endif /* entity_renderer_hpp */
