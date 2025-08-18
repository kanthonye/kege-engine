//
//  scene-module.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/17/25.
//

#ifndef scene_module_hpp
#define scene_module_hpp

#include "../scene/scene.hpp"
#include "module.hpp"

namespace kege{

    class SceneModule : public Module
    {
    public:

        const kege::Scene* operator ->()const;
        kege::Scene* operator ->();
        operator bool()const override;

        void setScene( kege::Ref< kege::Scene > scene );
        void changeScene( uint32_t scene_id );
        kege::Scene* getScene();

        bool initialize()override;
        void shutdown()override;
        void add()override;

        SceneModule( kege::Engine* engine );

        std::vector< std::string > _scene_files;
        kege::Ref< kege::Scene > _module;
    };

}
#endif /* scene_module_hpp */
