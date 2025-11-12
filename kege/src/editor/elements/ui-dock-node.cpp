//
//  ui-docking-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-dock-node.hpp"

namespace kege::ui{

    void DockNode::update(EditorLayer* mgr)
    {
        _layout->push( _main_container );
        if ( !_children.empty() )
        {
            for (auto i = _children.begin(); i != _children.end(); i++)
            {
                (*i)->update( mgr );
            }
        }
        else if( !_panels.empty() )
        {
            if (_tab_names.size() != _panels.size())
            {
                _tab_names.resize( _panels.size() );
                for (int i=0; i<_panels.size(); ++i)
                {
                    _tab_names[i] = _panels[i]->_name;
                }
            }
            _tabs.update(_layout, _tab_names);
            _panels[ _tabs.selection ]->update();
        }
        _layout->pop();
    }

    DockNode::DockNode(ui::Layout* layout, const std::string& split)
    :   _layout( layout )
    {
        _main_container = _layout->make
        ({
            .mouseover = false,
            .style = _layout->getStyleByName( split )
        });
    }

}
