//
//  entity-system-manager.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 8/13/25.
//

#ifndef kege_entity_system_manager_hpp
#define kege_entity_system_manager_hpp

#include "scene.hpp"
#include "render-executor.hpp"
#include "../app/project-manager.hpp"
#include "../input/input-context-manager.hpp"

namespace kege{

    class EntitySystem;

    class EntitySystemManager : public kege::RefCounter
    {
    public:

        template< typename SystemT > SystemT* addSystem()
        {
            SystemT* system = new SystemT( this );
            insert( system );
            return system;
        }

        void addSystem( kege::Ref< kege::EntitySystem > system );
        void addSystem( const std::string& name );
        
        void update( double dms );
        void render( double dms );
        void input( double dms );
        bool initialize();
        void shutdown();

        void operator () ( kege::Scene::Changed& msg );

        kege::RenderExecutor* getRenderExecutor();
        kege::AssetManager* getAssetManager();
        kege::MappedInputs& getMappedInputs();
        kege::Graphics* getGraphics();
        kege::Scene* getScene();

        EntitySystemManager( kege::InputContextManager* icm, kege::RenderExecutor* rm, kege::ProjectManager* pm );
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

        kege::InputContextManager* _input_context_manager;
        kege::ProjectManager* _project_manager;
        kege::RenderExecutor* _render_executor;
    };

    typedef EntitySystemManager ESM;
}

namespace kege::ref{
    typedef kege::Ref< kege::EntitySystemManager > EntitySystemManager;
}
#endif // kege_entity_system_manager_hpp
