//
//  main-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/24/25.
//

#include "main-panel.hpp"
#include "navbar-panel.hpp"

namespace kege{

    void MainPanel::update()
    {
        _layout->push( _main );
        for (ref::EditorPanel& panel : _panels )
        {
            panel->update();
        }
//            _navbar_panel.put( _layout );
//
//            _layout.push( _layout.make({ .visible = true, .style = _layout.getStyleByName( "content" ) }) );
//            {
//                _viewport_panel.put( _layout );
//
//                _layout.push( _layout.make({ .visible = true, .style = _layout.getStyleByName( "side-panel" ) }) );
//                {
//                    _hierarchy_panel.put( _layout );
//
//                    _layout.push( _layout.make({ .style = _layout.getStyleByName( "inspector-panel" ) }) );
//                    {
//                        _inspector_panel.put( _layout );
//                    }
//                }
//                _layout.pop();
//            }
//            _layout.pop();
        _layout->pop();
    }

    MainPanel::MainPanel( kege::ProjectManager* pm, ui::Layout* l )
    :   kege::EditorPanel( pm, l )
    {
    }

}
