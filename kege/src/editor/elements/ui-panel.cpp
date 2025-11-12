//
//  ui-panel.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-panel.hpp"

namespace kege::ui{

    Panel::Panel( const std::string& name, kege::ProjectManager* pm, ui::Layout* l )
    :   _name( name  )
    ,   _project_manager( pm )
    ,   _layout( l )
    {}
}
