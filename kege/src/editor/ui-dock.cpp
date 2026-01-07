//
//  ui-docking-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-dock.hpp"

namespace kege::ui{

    Dock::Dock()
    :   parent(nullptr)
    ,   padding{0,0,0,0}
    {
    }

    DockSplit::DockSplit(Dock& dock, float slit_ratio, ui::AlignDir direction)
    :   slit_ratio( slit_ratio )
    ,   direction( direction )
    ,   gap( 6 )
    {
        nodes[0].parent = &dock;
        nodes[1].parent = &dock;
        if (this->direction == ui::AlignDir::VERTICAL)
        {
            float height = dock.rect.height - (gap + dock.padding.above + dock.padding.below);

            nodes[0].rect.height = height * slit_ratio;
            nodes[0].rect.width  = dock.rect.width - (dock.padding.left + dock.padding.above);
            nodes[0].rect.x      = dock.rect.x + dock.padding.left;
            nodes[0].rect.y      = dock.rect.y + dock.padding.above;

            nodes[1].rect.height = height - nodes[0].rect.height;
            nodes[1].rect.width  = nodes[0].rect.width;
            nodes[1].rect.x      = nodes[0].rect.x;
            nodes[1].rect.y      = nodes[0].rect.y + nodes[0].rect.height + gap;

            updateVerticalDragRect();
        }
        else
        {
            float width = dock.rect.width - (gap + dock.padding.left + dock.padding.right);

            nodes[0].rect.width  = width * slit_ratio;
            nodes[0].rect.height = dock.rect.height - (dock.padding.above + dock.padding.below);
            nodes[0].rect.x      = dock.rect.x + dock.padding.left;
            nodes[0].rect.y      = dock.rect.y + dock.padding.above;

            nodes[1].rect.width  = width - nodes[0].rect.width;
            nodes[1].rect.height = nodes[0].rect.height;
            nodes[1].rect.x      = nodes[0].rect.x + nodes[0].rect.width + gap;
            nodes[1].rect.y      = nodes[0].rect.y;

            updateHorizontalDragRect();
        }
    }

    void DockSplit::updateHorizontalDragRect()
    {
        rect.x = nodes[0].rect.x + nodes[0].rect.width;
        rect.y = nodes[0].rect.y;
        rect.width = nodes[1].rect.x - rect.x;
        rect.height = nodes[0].rect.height;
    }

    void DockSplit::updateVerticalDragRect()
    {
        rect.y = nodes[0].rect.y + nodes[0].rect.height;
        rect.x = nodes[0].rect.x;
        rect.width = nodes[0].rect.width;
        rect.height = nodes[1].rect.y - rect.y;
    }

    void DockSplit::onResizeGrandChild(ui::AlignDir align_direction, const Rect& drag_rect, Dock& dock)
    {
        float closest_1, closest_2, length;

        if (align_direction == ui::AlignDir::VERTICAL && direction == ui::AlignDir::VERTICAL)
        {
            closest_1 = drag_rect.y - nodes[0].rect.y;
            closest_2 = drag_rect.y - nodes[1].rect.y;
            if (closest_1 < closest_2)
            {
                nodes[0].rect.y = dock.rect.y + dock.padding.above;
                nodes[0].rect.height = dock.rect.height - nodes[1].rect.height - gap - dock.padding.above;
                length = nodes[1].rect.height;

                if ( nodes[0].split ) nodes[0].split->onResize(align_direction, drag_rect, nodes[0]);
            }
            else
            {
                nodes[1].rect.y = nodes[0].rect.y + gap + nodes[0].rect.height;
                nodes[1].rect.height = dock.rect.height - nodes[0].rect.height - gap - dock.padding.below;
                length = nodes[0].rect.height;

                if ( nodes[1].split ) nodes[1].split->onResize(align_direction, drag_rect, nodes[1]);
            }
            slit_ratio = length / (dock.rect.height - dock.padding.above - dock.padding.below - gap);
        }

        else if (align_direction == ui::AlignDir::HORIZONTAL && direction == ui::AlignDir::HORIZONTAL)
        {
            closest_1 = drag_rect.x - nodes[0].rect.x;
            closest_2 = drag_rect.x - nodes[1].rect.x;

            if (closest_1 < closest_2)
            {
                nodes[0].rect.x = dock.rect.x + dock.padding.left;
                nodes[0].rect.width = dock.rect.width - nodes[1].rect.width - gap - dock.padding.left;
                length = nodes[1].rect.width;

                if ( nodes[0].split ) nodes[0].split->onResize(align_direction, drag_rect, nodes[0]);
            }
            else
            {
                nodes[1].rect.x = nodes[0].rect.x + nodes[0].rect.width + gap;
                nodes[1].rect.width = dock.rect.width - nodes[0].rect.width - gap - dock.padding.right;
                length = nodes[0].rect.width;

                if ( nodes[1].split ) nodes[1].split->onResize(align_direction, drag_rect, nodes[1]);
            }
            slit_ratio = length / (dock.rect.width - dock.padding.left - dock.padding.right - gap);
        }

        else
        {
            onResize(align_direction, drag_rect, dock);
        }
    }

    void DockSplit::onResize(ui::AlignDir align_direction, const Rect& drag_rect, Dock& dock)
    {
        if (direction == ui::AlignDir::VERTICAL)
        {
            float height = dock.rect.height
                         - gap
                         - dock.padding.above
                         - dock.padding.below;

            float h0 = height * slit_ratio;
            float h1 = height - h0;

            nodes[0].rect = {
                dock.rect.x + dock.padding.left,
                dock.rect.y + dock.padding.above,
                dock.rect.width - dock.padding.left - dock.padding.right,
                h0
            };

            nodes[1].rect = {
                nodes[0].rect.x,
                nodes[0].rect.y + h0 + gap,
                nodes[0].rect.width,
                h1
            };

            updateVerticalDragRect();
        }
        else
        {
            float width = dock.rect.width
                        - gap
                        - dock.padding.left
                        - dock.padding.right;

            float w0 = width * slit_ratio;
            float w1 = width - w0;

            nodes[0].rect = {
                dock.rect.x + dock.padding.left,
                dock.rect.y + dock.padding.above,
                w0,
                dock.rect.height - dock.padding.above - dock.padding.below
            };

            nodes[1].rect = {
                nodes[0].rect.x + w0 + gap,
                nodes[0].rect.y,
                w1,
                nodes[0].rect.height
            };

            updateHorizontalDragRect();
        }

        if (nodes[0].split)
        {
            nodes[0].split->onResizeGrandChild(align_direction, drag_rect, nodes[0]);
        }
        if (nodes[1].split)
        {
            nodes[1].split->onResizeGrandChild(align_direction, drag_rect, nodes[1]);
        }
    }

    bool DockSplit::update(GUI* gui, Dock& dock)
    {
        if (!gui->buttonDown())
            drag_resize = gui->testPointVsRect(gui->pointer(), rect);

        if (drag_resize)
        {
            if( gui->pointerDragging() )
            {
                float amount, available, delta_ratio;
                if( direction == AlignDir::VERTICAL )
                {
                    amount = gui->deltaPointer().y;
                    available = dock.rect.height - gap - dock.padding.above - dock.padding.below;
                    delta_ratio = amount / available;
                }
                else
                {
                    amount = gui->deltaPointer().x;
                    available = dock.rect.width - gap - dock.padding.left - dock.padding.right;
                    delta_ratio = amount / available;
                }
                slit_ratio = clamp(slit_ratio + delta_ratio, 0.05f, 0.95f);

                onResize(direction, rect, dock);
            }
        }
        return drag_resize;
    }
}


namespace kege::ui{

    void DockManager::split(Dock& dock, ui::AlignDir dir, const std::vector< Ref< ui::Panel > >& a, const std::vector< Ref< ui::Panel > >& b)
    {
        // Can't split a non-leaf node into a different direction, without first merging
        if (dock.split)
        {
            return;
        }

        dock.split = new DockSplit(dock, 0.5, dir);
        setDockPanels( dock.split->nodes[0], a );
        setDockPanels( dock.split->nodes[1], b );
    }

    void DockManager::merge(Dock& dock)
    {
        if ( dock.split )
        {
            merge(dock.split->nodes[0]);
            merge(dock.split->nodes[1]);
            for (int c=0; c<2; ++c)
            {
                for (int i=0; i<dock.split->nodes[c].tab.list.size(); ++i)
                {
                    dock.tab.list.push_back( dock.split->nodes[c].tab.list[i]) ;
                    dock.tab.panels.push_back( dock.split->nodes[c].tab.panels[i] );
                }
            }
            dock.split = {};
        }
    }

    void DockManager::setDockPanels(ui::Dock& node, const std::vector< Ref< ui::Panel > >& panels)
    {
        node.tab.panels = panels;
        for (int i=0; i<panels.size(); ++i)
        {
            node.tab.list.push_back(TabElem({ .text = _gui->layout()->text(panels[i]->_name.c_str(), 20) }));
        }
    }

    void DockManager::addPanelToDock(ui::Dock& node, const Ref< ui::Panel >& panel)
    {
        node.tab.panels.push_back( panel );
        node.tab.list.push_back(TabElem({ .text = _gui->layout()->text(panel->_name.c_str(), 20) }));
    }

    void DockManager::switchSpot(ui::Dock& a, ui::Dock& b)
    {
        Tab tab = a.tab;
        a.tab = b.tab;
        b.tab = tab;
    }

    void DockManager::ghostDraggingOp(ui::Layout* layout, const ui::UID& id, void* data)
    {
        GhostParam* params = reinterpret_cast<GhostParam*>(data);
        if (!params->ghost->visible)
        {
            Widget* target = layout->elem(*params->ghost->target);
            Widget* ghost  = layout->elem(id);

            params->ghost->visible = true;
            params->ghost->rect.x = target->rect.x;
            params->ghost->rect.y = target->rect.y;
            params->ghost->rect.width = ghost->rect.width;
            params->ghost->rect.height = ghost->rect.height;
        }
        params->ghost->rect.x += layout->input()->deltaPosition().x;
        params->ghost->rect.y += layout->input()->deltaPosition().y;
    }

    void DockManager::ghostDropoffOp(ui::Layout* layout, const ui::UID& id, void* data)
    {
        GhostDropoff* params = reinterpret_cast<GhostDropoff*>(data);
        params->manager->handleDropOff(params->manager->_root);
    }

    void DockManager::updateTabs(ui::Dock& dock)
    {
        _gui->push({.layer = 0, .style = &_styles[ PANEL ]});
        {
            _gui->push
            ({
                .layer = 0,
                .uid = &dock.tab.uid,
                .style = &_styles[ TAB_LIST ]
            });
            for (int i=0; i<dock.tab.list.size(); ++i)
            {
                _gui->push
                ({
                    .layer = 0,
                    .style = &_styles[ TAB_ELEM ],
                    .uid = &dock.tab.list[i].uids[0],
                    .single_click = ui::ClickTrigger::Continuous,
                    .border.corner_curves = {8,8,0,0},
                });
                _gui->put
                ({
                    .layer = 0,
                    .style = &_styles[ TAB_LABEL ],
                    .uid = &dock.tab.list[i].uids[1],
                    .text = dock.tab.list[i].text,
                    .enabled = false,
                });
                _gui->put
                ({
                    .layer = 0,
                    .uid = &dock.tab.list[i].uids[2],
                    .text = _gui->layout()->text("x", 20),
                    .single_click = ui::ClickTrigger::OnRelease,
                    .style = (!_gui->hot(dock.tab.list[i].uids[2])) ? &_styles[ CLOSE ]: &_styles[ CLOSE_HOT ],
                });
                _gui->pop(0);

                if( _gui->click( dock.tab.list[i].uids[0] ) )
                {
                    dock.tab.selection = i;
                    if (!_ghost.dragging)
                    {
                        _ghost.dragging = true;
                        _ghost.dock = &dock;
                        _ghost.text = dock.tab.list[i].text;
                        _ghost.target = &dock.tab.list[i].uids[0];
                    }
                }
            }
            _gui->pop(0);
            
            dock.tab.panels[ dock.tab.selection ]->update();
        }
        _gui->pop(0);
    }

    void DockManager::update(ui::Dock& dock)
    {
        _gui->push
        ({
            .layer = 0,
            .uid = &dock.uid,
            .rect = dock.rect,
            .position = ui::Positioning::Independent,
            //.color = 0xFF44FF30,
            .padding = dock.padding,
        });
        if ( dock.split )
        {
            if ( dock.split->update(_gui, dock) )
            {
                _gui->put
                ({
                    .layer = 1,
                    .rect = dock.split->rect,
                    .color = 0xFF0000FF,
                });
            }

            update(dock.split->nodes[0]);
            update(dock.split->nodes[1]);
        }
        else if( !dock.tab.panels.empty() )
        {
            updateTabs(dock);
        }
        _gui->pop(0);
    }
    
    void DockManager::update()
    {
        update(_root);
        if( _ghost.dragging )
        {
            if( !_gui->buttonDown() )
            {
                _ghost.visible = false;
                _ghost.dragging = false;
                _gui->pushDeferredOp<GhostDropoff>(uid, ghostDropoffOp, GhostDropoff{ &_ghost, this });
            }
            else if( _gui->pointerDragging() )
            {
                _gui->pushDeferredOp<GhostParam>(uid, ghostDraggingOp, GhostParam{ &_ghost });
                _gui->put
                ({
                    .layer = 1,
                    .uid = &uid,
                    .text = _ghost.text,
                    .rect.x = _ghost.rect.x,
                    .rect.y = _ghost.rect.y,
                    .border.corner_curves = {8,8,8,8},
                    .style = &_styles[ DOCK_PREVIEW ],
                });
            }
        }
    }

    void DockManager::handleSiblingDropSpot(ui::Dock& node, float lr, float tb)
    {
//        if ( &node.parent->children->nodes[0] == _ghost.dock )
//        {
//            /**
//             If the node is split horizontally then the nodes are aligned one on top, one on bottom,
//             */
//            if (node.parent->style.align.direction == ui::AlignDir::VERTICAL)
//            {
//                if (abs(tb) < abs(lr))
//                {
//                    node.parent->style.align.direction = AlignDir::HORIZONTAL;
//                    if (0 < lr) // dropoff is on the RIGHT of the node
//                    {
//                        switchSpot(node, *_ghost.dock);
//                        //split(node, ui::AlignDir::VERTICAL, node.panels, _ghost.dock->panels);
//                    }
//                }
//                else
//                {
//                    if (0 < tb) // dropoff is on the Top of the node
//                    {
//                        // maintain the same order of alignment, no change is made
//                        return;
//                    }
//                    else // dropoff is on the BOTTOM of the node
//                    {
//                        switchSpot(node, *_ghost.dock);
//                    }
//                }
//            }
//            /**
//             If the node is split vertically then the nodes are aligned one on left, one on right,
//             */
//            else // if (node.parent->split_dir == SplitDirection::VERTICAL)
//            {
//                if (abs(tb) < abs(lr))
//                {
//                    if (0 < lr) // dropoff is on the RIGHT of the node
//                    {
//                        // maintain the same order of alignment, no change is made
//                        return;
//                    }
//                    else  // dropoff is on the LEFT of the node
//                    {
//                        switchSpot(node, *_ghost.dock);
//                    }
//                }
//                else
//                {
//                    node.parent->style.align.direction = AlignDir::VERTICAL;
//                    if (0 > tb) // dropoff is on the Top of the node
//                    {
//                        switchSpot(node, *_ghost.dock);
//                    }
//                }
//            }
//        }
//        else if ( &node.parent->children->nodes[1] == _ghost.dock )
//        {
//            /**
//             If the node is split horizontally then the nodes are aligned one on top, one on bottom,
//             */
//            if (node.parent->style.align.direction == ui::AlignDir::VERTICAL)
//            {
//                if (abs(tb) < abs(lr))
//                {
//                    node.parent->style.align.direction = AlignDir::HORIZONTAL;
//                    if (node.style.height.type == SizingType::SIZE_PERCENT)
//                    {
//                        node.style.height.type = SizingType::SIZE_EXTEND;
//                    }
//                    if (0 > lr) // dropoff is on the RIGHT of the node
//                    {
//                        switchSpot(node, *_ghost.dock);
//                    }
//                }
//                else
//                {
//                    if (0 < tb) // dropoff is on the Top of the node
//                    {
//                        switchSpot(node, *_ghost.dock);
//                    }
//                    else // dropoff is on the BOTTOM of the node
//                    {
//                        // maintain the same order of alignment, no change is made
//                        return;
//                    }
//                }
//            }
//            /**
//             If the node is split vertically then the nodes are aligned one on left, one on right,
//             */
//            else // if (node.parent->split_dir == SplitDirection::VERTICAL)
//            {
//                if (abs(tb) < abs(lr))
//                {
//                    if (0 < lr) // dropoff is on the Right of the node
//                    {
//                        // maintain the same order of alignment, no change is made
//                        return;
//                    }
//                    else // dropoff is on theLeft of the node
//                    {
//                        switchSpot(node, *_ghost.dock);
//                    }
//                }
//                else
//                {
//                    node.parent->style.align.direction = AlignDir::VERTICAL;
//                    if (0 < tb) // dropoff is on the Top of the node
//                    {
//                        switchSpot(node, *_ghost.dock);
//                    }
//                }
//            }
//        } Text{ Data{const char* str, int size, modified = false}* data; }
    }

    bool DockManager::handleDropOff(ui::Dock& dock)
    {
        Widget* ui = _gui->get( dock.uid );

        /**
         test if the drop spot is the given node, if not return false, otherwise proceed
         */
        if (!textRect(ui->rect, _ghost.rect))
        {
            return false;
        }

        /**
         if the given node has children, one of the child nodes is the drop spot
         */
        if ( dock.split )
        {
            if( handleDropOff(dock.split->nodes[0]) ) return true;
            if( handleDropOff(dock.split->nodes[1]) ) return true;
            return false;
        }

        /**
         * if the drop off spot is the same as the drag origin, then if there is more than one panel return true
         */
        if ( dock.uid.global == _ghost.dock->uid.global )
        {
            /**
             * if tabs are greater than 1 then one of these tabs are the tab being dropped off,
             * so need to split node with one tab in each.
             */
            if ( 2 <= _ghost.dock->tab.panels.size() )
            {
                kege::vec2 center;
                center.x = dock.rect.x + dock.rect.width * 0.5f;
                center.y = dock.rect.y + dock.rect.height * 0.5f;

                kege::vec2 v = kege::normalize(kege::vec2(_gui->pointer().x,_gui->pointer().y) - center);
                float tb = kege::dot(v, kege::vec2(0.f,1.f));
                float lr = kege::dot(v, kege::vec2(1.f,0.f));

                Ref< ui::Panel > panel = dock.tab.panels[ _ghost.dock->tab.selection ];
                dock.tab.panels.erase(dock.tab.panels.begin() + _ghost.dock->tab.selection);

                if (abs(lr) < abs(tb))
                {
                    if (0 < tb) // dropoff is on the RIGHT of the node
                    {
                        split(dock, ui::AlignDir::VERTICAL, dock.tab.panels, {panel});
                    }
                    else  // dropoff is on the LEFT of the node
                    {
                        split(dock, ui::AlignDir::VERTICAL, {panel}, dock.tab.panels);
                    }
                }
                else
                {
                    if (0 < lr) // dropoff is on the Top of the node
                    {
                        split(dock, ui::AlignDir::HORIZONTAL, dock.tab.panels, {panel});
                    }
                    else // dropoff is on the BOTTOM of the node
                    {
                        split(dock, ui::AlignDir::HORIZONTAL, {panel}, dock.tab.panels);
                    }
                }

                _ghost.dock->tab.panels.clear();
                _ghost.dock->tab.list.clear();
                _ghost.dock->tab.selection = 0;

                dock.tab.panels.clear();
                dock.tab.list.clear();
                dock.tab.selection = 0.f;
            }
            return true;
        }

        /**
         if the drop off spot is the tab area, then add the panel to the dropoff target tablist
         */
        Widget* widget_tab = _gui->get( dock.tab.uid );
        if (textRect(widget_tab->rect, _ghost.rect))
        {
            /**
             check if the drop spot already contain the panel being dropped off.
             if it has that panel exit
             */
            size_t min_count = kege::min(dock.tab.panels.size(), _ghost.dock->tab.panels.size());
            for(int i=0; i<min_count; ++i)
            {
                if ( _ghost.dock->tab.panels[i]->getName() == dock.tab.panels[i]->getName() )
                {
                    return true;
                }
            }

            /**
             if the drop off target and the origin has the same parent merge them
             */
            if ( dock.parent == _ghost.dock->parent && dock.parent != nullptr && _ghost.dock->tab.panels.size() == 1 )
            {
                merge(*dock.parent);
                return true;
            }
            /**
             otherwise add panel to dropoff target tablist
             */
            dock.tab.list.push_back(TabElem({ .text = _ghost.text }));
            dock.tab.panels.push_back( _ghost.dock->tab.panels[ _ghost.dock->tab.selection ] );
            /**
             remove panel from old tablist
             */
            _ghost.dock->tab.panels.erase( _ghost.dock->tab.panels.begin() + _ghost.dock->tab.selection );
            _ghost.dock->tab.list.erase( _ghost.dock->tab.list.begin() + _ghost.dock->tab.selection );

            /**
             if old tablist is empty then old dock parent shold merge into one
             */
            if ( _ghost.dock->tab.list.empty() )
            {
                Dock* parent = _ghost.dock->parent;

                Dock& survivor = (parent->split->nodes[0].tab.panels.empty() && !parent->split->nodes[0].split)
                ? parent->split->nodes[1]
                : parent->split->nodes[0];

                Ref< DockSplit > split = survivor.split;

                if (split)
                {
                    parent->split = split;
                    parent->split->nodes[0].parent = parent;
                    parent->split->nodes[1].parent = parent;
                    split->onResize(split->direction, split->rect, *parent);
                }
                else
                {
                    parent->tab.panels = std::move( survivor.tab.panels );
                    parent->tab.list = std::move( survivor.tab.list );
                    parent->name = survivor.name;
                    parent->split = {};
                }
            }
            else
            {
                _ghost.dock->tab.selection = _ghost.dock->tab.selection % int(_ghost.dock->tab.list.size());
            }
        }
        else
        {
//            kege::vec2 center;
//            center.x = (dock->rect.x + dock->rect.width ) * 0.5f;
//            center.y = (dock->rect.y + dock->rect.height) * 0.5f;
//
//            kege::vec2 v = kege::normalize(kege::vec2(_gui->pointer().x,_gui->pointer().y) - center);
//            float tb = kege::dot(v, kege::vec2(0.f,-1.f));
//            float lr = kege::dot(v, kege::vec2(1.f,0.f));
//
//            if ( node.parent != nullptr )
//            {
//                /**
//                 if the node being drag is drop into its sibling the merge them into one node.
//                 */
//                if ( &node.parent->children->nodes[0] == _ghost.dock || &node.parent->children->nodes[1] == _ghost.dock )
//                {
//                    handleSiblingDropSpot( node, lr, tb );
//                    return true;
//                }
//            }
//
////            if (abs(tb) < abs(lr))
////            {
////                if (0 < lr) // dropoff is on the RIGHT of the node
////                {
////                    split(node, ui::AlignDir::VERTICAL, node.panels, _ghost.dock->panels);
////                }
////                else  // dropoff is on the LEFT of the node
////                {
////                    split(node, ui::AlignDir::VERTICAL, _ghost.dock->panels, node.panels);
////                }
////            }
////            else
////            {
////                if (0 < tb) // dropoff is on the Top of the node
////                {
////                    split(node, ui::AlignDir::HORIZONTAL, _ghost.dock->panels, node.panels);
////                }
////                else // dropoff is on the BOTTOM of the node
////                {
////                    split(node, ui::AlignDir::HORIZONTAL, node.panels, _ghost.dock->panels);
////                }
////            }
////            node.panels.clear();
////            node.tab.list.clear();
////            node.tab.selection = 0.f;
////
////            _ghost.dock->panels.clear();
////            _ghost.dock->tab.list.clear();
////            _ghost.dock->tab.selection = 0;
////
////            if ( _ghost.dock->parent )
////            {
////                Ref< DockNode > children = node.parent->children;
////                _ghost.dock->parent->style.align.direction = node.parent->style.align.direction;
////                _ghost.dock->parent->children = children;
////            }
        }
        return true;
    }

    bool DockManager::textRect(const Rect& m, const Rect& n)
    {
        return
        (
            m.x < n.x + n.width &&  // rect1 left edge is left of rect2 right edge
            m.x + m.width > n.x &&  // rect1 right edge is right of rect2 left edge
            m.y < n.y + n.height && // rect1 top edge is above rect2 bottom edge
            m.y + m.height > n.y    // rect1 bottom edge is below rect2 top edge
        );
    }

    ui::Dock& DockManager::getRoot()
    {
        return _root;
    }

    DockManager::DockManager(kege::GUI* gui, int width, int height)
    :   _gui( gui )
    {
        _root.padding = {10,10,10,10};
        _root.parent = nullptr;
        _root.rect.height = height;
        _root.rect.width = width;
        _root.rect.x = 0.f;
        _root.rect.y = 0.f;
        _root.name = "root";

        _styles[PANEL].width = ui::extend();
        _styles[PANEL].height = ui::extend();
        _styles[PANEL].padding = {};
        _styles[PANEL].background = ui::Background(0x0b090fFF);
        _styles[PANEL].align.direction = ui::AlignDir::VERTICAL;
        _styles[PANEL].align.flow = {ui::AlignDirX::WTE, ui::AlignDirY::NTS};
        _styles[PANEL].align.origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP};
        _styles[PANEL].align.content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP};

        _styles[TAB_LIST] = kege::ui::Style{
            .background = ui::Background(0x0B090F00),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {4,0},
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
            .padding = {10,4,4,4},
            .gap = {20,0}
        };
        _styles[ TAB_LABEL ] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::flexible(),
            .background = ui::Background(0xFFFFFF00),
            .align_text =  ui::AlignText::Left,
            .text_color = 0xBBA0FFFF,
            .font_size = 20,
        };
        _styles[ CLOSE ] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text =  ui::AlignText::Center,
            .text_color = 0xBBA0FFFF,
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .font_size = 20,
            .border.corner_curves = {2,2,2,2},
        };
        _styles[ CLOSE_HOT ] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF20),
            .align_text = ui::AlignText::Center,
            .text_color = 0xBBA0FFFF,
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .font_size = 20,
            .border.corner_curves = {4,4,4,4},
        };

        _styles[ DOCK_PREVIEW ] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF20),
            .align_text =  ui::AlignText::Center,
            .position = ui::Positioning::Absolute,
            .text_color = 0xBBA0FFFF,
            .height = ui::fixed(40),
            .width = ui::fixed(100),
            .font_size = 20,
            .border.corner_curves = {8,8,8,8},
        };
    }

}
