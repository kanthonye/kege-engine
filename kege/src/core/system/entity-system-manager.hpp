//
//  entity-system-manager.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/13/25.
//

#ifndef entity_system_manager_hpp
#define entity_system_manager_hpp

#include "../../core/system/system.hpp"

namespace kege{

    class EntitySystem;

    class EntitySystemManager : public kege::System
    {
    public:

        template< typename SystemT > SystemT* addSystem()
        {
            SystemT* system = new SystemT( _engine );
            insert( system );
            return system;
        }

        EntitySystemManager( kege::Engine* engine );

        void addSystem( kege::Ref< kege::EntitySystem > system );
        void addSystem( const std::string& name );
        void update( double dms );
        void render( double dms );
        void onSceneChange();
        bool initialize();
        void shutdown();

        virtual ~EntitySystemManager();

    protected:

        std::vector< kege::Ref< kege::EntitySystem > > _systems;

        /**
         * systems that require their update function to be called
         */
        std::vector< kege::EntitySystem* > _system_updates;

        /**
         * systems that requires their render function to be called
         */
        std::vector< kege::EntitySystem* > _system_renders;

        /**
         * systems that requires their input function to be called
         */
        std::vector< kege::EntitySystem* > _system_inputs;

        kege::Engine* _engine;
    };

    typedef EntitySystemManager ESM;
}

#endif /* entity_system_manager_hpp */
