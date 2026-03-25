//
//  ui-panel.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-panel.hpp"
#include "../editor-layer.hpp"

namespace kege::ui{

    Panel::Panel( const std::string& name, kege::EditorLayer* e )
    :   _name( name  )
    ,   _editor( e )
    ,   _gui(e->getGUI())
    {
    }
}
