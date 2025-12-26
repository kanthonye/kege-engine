//
//  ui-docking-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-dock.hpp"

namespace kege::ui{

    Dock::Dock()
    {
        style = kege::ui::Style
        {
            .background = ui::Background(0x000000FF),
            //.padding = {20,20, 20,20},
            .height = ui::extend(),
            .width = ui::extend(),
            .gap = {4,4},
            .align =
            {
                .content = {ui::AlignPosX::CENTER, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
    }

}


namespace kege::ui{

    Ref< ui::Dock > DockManager::create( const std::string& name )
    {
        Ref< Dock > dock = new Dock;
        _nodes.push_back( dock );
        dock->name = name;
        return dock;
    }

    void DockManager::dock(Ref< ui::Dock > node, DockZone zone, Ref< ui::Panel > panel)
    {
        node->panels.push_back( panel );
        node->tabs.push_back(DockTab({ .name = panel->_name.c_str() }));
    }

    void DockManager::splitNode(Dock* node, float split_ratio, SplitDirection dir)
    {
        // Can't split a non-leaf node into a different direction
        // without first merging
        if (node->count != 0)
        {
            return;
        }

        // Create two children
        node->child[0] = create(node->name + "-left");
        node->child[1] = create(node->name + "-right");
        node->split_ratio = split_ratio;
        node->split_dir = dir;
        node->count = 2;

        if( dir == SplitDirection::HORIZONTAL )
        {
            node->style.align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            };
            node->child[0]->style.height = ui::percent( split_ratio * 100 );
        }
        else
        {
            node->style.align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            };
            node->child[0]->style.width = ui::percent( split_ratio * 100 );
        }

        // Set up parent relationships
        //node->child[0]->parent = node;
        //node->child[1]->parent = node;

        // Move existing window to first child if present
        if (node->panels.empty())
        {
            node->child[0]->panels = node->panels;
            //for (int i=0; i<node->panels.size(); ++i) {
            //    node->child[0]->panels[i]->dock = node->child[0].ref();
            //}
            node->panels.clear();
        }
    }

    void DockManager::mergeNode(Dock* node)
    {}

    void DockManager::beginDrag(Dock* node)
    {}

    void DockManager::handleDrop()
    {}

    void DockManager::updateTabs(Ref< ui::Dock >& node)
    {
        _gui->push({ .style = &_styles[ TAB_LIST ] });
        for (int i=0; i<node->tabs.size(); ++i)
        {
            _gui->push
            ({
                .style = &_styles[ TAB_ELEM ],
                .id = &node->tabs[i].uids[0],
                .single_click = ui::ClickTrigger::Continuous,
            });
            _gui->put
            ({
                .style = &_styles[ TAB_LABEL ],
                .id = &node->tabs[i].uids[1],
                .text = node->tabs[i].name.c_str(),
                .enabled = false,
            });
            _gui->put
            ({
                .id = &node->tabs[i].uids[2],
                .text = "x",
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (!_gui->hot(node->tabs[i].uids[2])) ? &_styles[ CLOSE ]: &_styles[ CLOSE_HOT ],
            });
            _gui->pop();

            if( _gui->click( node->tabs[i].uids[0] ) )
            {
                node->selection = i;
                if (!_dragging)
                {
                    _dragging = true;
                    _offset = {};
                }
                _gui->get( node->tabs[i].uids[0] )->offset.x = _offset.x;
                _gui->get( node->tabs[i].uids[0] )->offset.y = _offset.y;
            }
        }
        _gui->pop();

        node->panels[ node->selection ]->update();
    }

    void DockManager::update(Ref< ui::Dock >& node)
    {
        _gui->push({ .id = &node->uid, .style = &node->style });
        if ( node->count != 0 )
        {
            update(node->child[0]);
            update(node->child[1]);
        }
        else if( !node->panels.empty() )
        {
            _gui->push({.style = &_styles[ PANEL ]});
            updateTabs(node);
            _gui->pop();
        }
        _gui->pop();
    }
    
    void DockManager::update()
    {
        update(_root);
        if( _dragging )
        {
            _offset.x += _gui->deltaPointer().x;
            _offset.y += _gui->deltaPointer().y;
            //_gui->put({ .id = &uid, .style = &_styles[ DOCK_PREVIEW ], .offset = _offset });
            //std::cout << _offset.x <<"," << _offset.y <<"\n";
            if( !_gui->buttonDown() )
            {
                _dragging = false;
            }
        }
    }

    ui::Dock* DockManager::getRoot()
    {
        return _root.ref();
    }

    DockManager::DockManager(kege::GUI* gui)
    :   _gui( gui )
    {
        _root = new Dock;
        _root->style.padding = {8,8, 8,8};
        _root->style.align =
        {
            .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
            .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
            .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
            .direction = ui::AlignDir::HORIZONTAL
        };

        _styles[PANEL] = kege::ui::Style{
            .border_radius = {5,5, 5,5},
            .background = ui::Background(0x0b090fFF),
            //.background = ui::Background(0xFF1420FF),
            .height = ui::extend(),
            .width = ui::extend(),
            .align =
            {
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
        _styles[TAB_LIST] = kege::ui::Style{
            .background = ui::Background(0x0B090F00),
            .height = ui::flexible(),
            .width = ui::extend(),
            .align =
            {
                .flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS},
                .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        _styles[ TAB_ELEM ] = kege::ui::Style{
            .height = ui::flexible(),
            .width = ui::flexible(),
            .background = ui::Background(0x171420FF),
            .align_text =  ui::AlignText::Center,
            .padding = {6,4,4,4},
            .gap = {8,0}
        };
        _styles[ TAB_LABEL ] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::flexible(),
            .background = ui::Background(0xFFFFFF00),
            .align_text =  ui::AlignText::Center,
            .color = ui::rgba(0xBBA0FFFF),
            .font_size = 20,
        };
        _styles[ CLOSE ] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text =  ui::AlignText::Center,
            .color = ui::rgba(0xBBA0FFFF),
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .font_size = 20,
            .border_radius = {2,2,2,2},
        };
        _styles[ CLOSE_HOT ] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF20),
            .align_text =  ui::AlignText::Center,
            .color = ui::rgba(0xBBA0FFFF),
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .font_size = 20,
            .border_radius = {4,4,4,4},
        };

        _styles[ DOCK_PREVIEW ] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF20),
            .align_text =  ui::AlignText::Center,
            .position = ui::Positioning::Absolute,
            .color = ui::rgba(0xBBA0FFFF),
            .height = ui::fixed(100),
            .width = ui::fixed(100),
            .font_size = 20,
            .border_radius = {4,4,4,4},
        };
        _offset = {};
    }

}
