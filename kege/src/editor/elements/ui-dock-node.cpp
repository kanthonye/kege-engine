//
//  ui-docking-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-dock-node.hpp"

namespace kege::ui{

    void DockNode::updateDock(EditorLayer* mgr)
    {
        if ( !_children.empty() )
        {
            for (auto i = _children.begin(); i != _children.end(); i++)
            {
                (*i)->update( mgr );
            }
        }
        else if( !_panels.empty() )
        {
            //_layout->push( _inner_container );
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
            //_layout->pop();
        }
    }

    void DockNode::update(EditorLayer* mgr)
    {
        _layout->push( _dock );
        if ( !_children.empty() )
        {
            for (auto i = _children.begin(); i != _children.end(); i++)
            {
                (*i)->update( mgr );
            }
        }
        else if( !_panels.empty() )
        {
            _layout->push( _container );
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
            _layout->pop();
        }
        _layout->pop();
    }

    void DockNode::split()
    {
        if ( _children.empty() )
        {
            _children.resize(2);
            _children[0] = new DockNode( _layout );
            _children[1] = new DockNode( _layout );
        }
    }

    DockNode::DockNode(ui::Layout* layout, const std::string& style)
    :   _layout( layout )
    {
        _dock = _layout->make
        ({
            .mouseover = false,
            .style = _layout->getStyleByName( style )
        });
        _container = _layout->make
        ({
            .mouseover = false,
            .style = _layout->getStyleByName( "dock-inner" )
        });
    }

    DockNode::DockNode(ui::Layout* layout)
    :   _layout( layout )
    {
        _dock = _layout->make
        ({
            .mouseover = false,
            .style = _layout->getStyleByName( "dock" )
        });
        _container = _layout->make
        ({
            .mouseover = false,
            .style = _layout->getStyleByName( "dock-inner" )
        });
    }

}
