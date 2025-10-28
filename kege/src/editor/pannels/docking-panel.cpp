//
//  docking-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "docking-panel.hpp"

namespace kege{

    void DockingPanel::add( ref::EditorPanel panel )
    {
        _panels.push_back( panel );
    }

    void DockingPanel::update( )
    {
        _layout->push( _main );
        for (ref::EditorPanel& panel : _panels )
        {
            panel->update();
        }
        _layout->pop();
    }

    DockingPanel::DockingPanel( kege::ProjectManager* pm, ui::Layout* l )
    :   kege::EditorPanel( pm, l )
    {
    }

}
