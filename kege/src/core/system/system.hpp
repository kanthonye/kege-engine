//
//  system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#ifndef kege_system_hpp
#define kege_system_hpp

#include "../memory/ref.hpp"
#include "../utils/log.hpp"
#include "../utils/communication.hpp"
#include "settings.hpp"
#include "../../../src/core/math/algebra/vectors.hpp"
#include "../../../src/core/math/algebra/matrices.hpp"
#include "../../../src/core/math/algebra/quaternion.hpp"
#include "../../../src/core/math/algebra/transform.hpp"
#include "../../../src/core/graphics/core/graphics.hpp"
#include "../../../src/core/graphics/graph/render-graph.hpp"
#include "../../../src/core/input/input-context-manager.hpp"

namespace kege{

    class Engine;

    class System : public kege::RefCounter
    {
    public:

        const std::string& getName() const;
        virtual bool initialize();
        virtual void shutdown();

        System( kege::Engine* engine, const std::string& name );
        virtual ~System();

        System( const System& ) = delete;
        System& operator=( const System& ) = delete;

    protected:

//        void setRenderGraph( kege::Ref< kege::RenderGraph > graphics );
//        void setWindow( kege::Ref< kege::GraphicsWindow > window );
//        void setGraphics( kege::Ref< kege::Graphics > graphics );

    protected:

        kege::Communication _comm;
        kege::Engine* _engine;
        std::string _name;

        friend kege::Engine;
    };

    struct MsgExecuteRenderProcess{};


    template < typename CoreModule > class CoreSystem: public System
    {
    public:

        CoreSystem( kege::Engine* engine, const std::string& name )
        :   kege::System( engine, name )
        {}

        CoreSystem(const CoreSystem&) = delete;
        CoreSystem& operator=(const CoreSystem&) = delete;

        virtual inline CoreModule* operator ->(){ return _module.ref(); }
        virtual inline CoreModule& operator *(){ return *_module.ref(); }
        virtual bool initialize() = 0;
        virtual void shutdown() = 0;

        virtual~ CoreSystem(){}

    protected:

        kege::Ref< CoreModule > _module;
    };


}


namespace kege{

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
        return kege::Ref< kege::System >( new kege::CLASSNAME( engine ) );      \
    }                                                                           \
    AutoRegister##CLASSNAME()                                                   \
    {                                                                           \
        SystemFactory::instance().registerSystem( NAME, createSystem );         \
    }                                                                           \
};                                                                              \
static AutoRegister##CLASSNAME register_##CLASSNAME;                            \


#endif /* system_hpp */
