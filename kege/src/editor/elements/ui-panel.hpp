//
//  ui-panel.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#ifndef ui_panel_hpp
#define ui_panel_hpp

#include "ui-tree-node.hpp"
#include "../../core/scene/ecs.hpp"
#include "../../core/scene/scene.hpp"
#include "../../core/app/project-manager.hpp"

namespace kege::ui{

    struct Panel : public kege::RefCounter
    {
        const std::string getName()const{ return _name; }
        virtual void update() = 0;

        Panel( const std::string& name, kege::ProjectManager* pm, ui::Layout* l, kege::ECS* e );
        virtual ~Panel(){}

        kege::ProjectManager* _project_manager;
        ui::Layout* _layout;
        kege::ECS* _ecs;
        std::string _name;
    };

}
#endif /* ui_panel_hpp */
