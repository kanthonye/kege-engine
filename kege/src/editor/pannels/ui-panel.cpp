//
//  ui-panel.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-panel.hpp"

namespace kege::ui{

    Panel::Panel( const std::string& name, kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* e )
    :   _name( name  )
    ,   _project_manager( pm )
    ,   _gui( gui )
    ,   _ecs( e )
    {}
}
