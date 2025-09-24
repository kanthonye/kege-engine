//
//  core-scene-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 9/8/25.
//

#ifndef core_scene_manager_hpp
#define core_scene_manager_hpp

#include "../scene/scene.hpp"
#include "module.hpp"

#include "../ecs/entity.hpp"
#include "../esm/entity-system-manager.hpp"

namespace kege{

    class SceneManager : public Module
    {
    public:

        void setScene( kege::Ref< kege::Scene > scene );
        void changeScene( uint32_t scene_id );
        kege::Scene* getScene();

        const kege::EntitySystemManager* operator ->()const;
        kege::EntitySystemManager* operator ->();
        operator bool()const override;

        void update( double dms );
        void render( double dms );
        void input( double dms );

        bool initialize()override;
        void shutdown()override;
        void add()override;

        SceneManager( kege::Engine* engine );

        kege::Ref< kege::EntitySystemManager > _entity_systems;
        kege::Ref< kege::EntityManager > _entity_manager;
        kege::Ref< kege::Scene > _scene;

        std::vector< std::string > _scene_files;
    };

}
#endif /* core_scene_manager_hpp */
