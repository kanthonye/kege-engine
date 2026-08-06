//
//  ui-docking-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-dock.hpp"
#include "ui-dock-split.hpp"
#include "ui-dock-manager.hpp"
#include "../editor-layer.hpp"

namespace kege::ui{

    void Dock::operator()(const kege::ui::AssetMetadataDropOff& event)
    {
        kege::ui::Dock* dock = getDock(*this, event.position);
        if ( dock != nullptr )
        {
            uint32_t panel_index = dock->_tab.panel_indices[ dock->_tab.selection ];
            ui::Panel* panel = _manager->getPanel( panel_index );
            panel->operator()( event );
        }
    }

    kege::ui::Dock* Dock::getDock(Dock& dock, const kege::dvec2& pointer)
    {
        kege::ui::Dock* res = nullptr;
        if( kege::ui::testPointVsRect(pointer, dock._quad) )
        {
            if (dock._split)
            {
                if ((res = getDock(dock._split->nodes[0], pointer)) != nullptr) return res;
                if ((res = getDock(dock._split->nodes[1], pointer)) != nullptr) return res;
            }
            return &dock;
        }
        return res;
    }

    kege::ui::Dock* Dock::getDock(const kege::dvec2& pointer)
    {
        return getDock(*this, pointer);
    }

    void Dock::displayTabs()
    {
        _ui->push
        ({
            .wid = _ui->newElem( _ui->theme()->dock ),
            .name = "tab-body"
        });
        {
            _ui->push
            ({
                .user_id = _uid_tab[0],
                .wid = _ui->newElem(_ui->theme()->tab)
            });
            for (int i=0; i<_tab.list.size(); ++i)
            {
                TabElem& elem = _tab.list[i];
                ui::ID id[3] = {_uid_tab[i + 1], _uid_tab[i + 2], _uid_tab[i + 3]};

                _ui->push
                ({
                    .user_id = id[0],
                    .wid = _ui->newElem(_ui->theme()->tab_elem),
                    .single_click = ui::ClickTrigger::Continuous,
                    .name = "tab-button"
                });
                _ui->put
                ({
                    .user_id = id[2],
                    .wid = _ui->newElem({
                        .quad_color = _ui->mouseover( id[2] ) ? 0xEE2200FF : 0xEE220050,
                        .corner_curves = {8,8,8,8},
                    }),
                    .quad = { .y = 4, .width = 10, .height = 10 },
                    .single_click = ui::ClickTrigger::OnRelease,
                    .double_click = ui::ClickTrigger::Immediate,
                    .name = "tab-b2"
                });
                _ui->put
                ({
                    .user_id = id[1],
                    .wid = _ui->newElem(_ui->theme()->tab_label),
                    //.quad = {0,0, 30, static_cast<float>(elem.text.font_size)},
                    .text = elem.text,
                    .mouseover = false,
                    .name = "tab-name"
                });
                _ui->pop();

                if( _ui->click( id[0] ) )
                {
                    _tab.selection = i;
                    if (!_manager->getGhostObject()->dragging && _ui->pointerDragging())
                    {
                        _manager->getGhostObject()->dragging = true;
                        _manager->getGhostObject()->dock = this;
                        _manager->getGhostObject()->tab_selection = i;
                    }
                }
            }
            _ui->pop();

            _manager->displayPanel( _tab.panel_indices[ _tab.selection ] );
            ////_context->panels[ _tab.panel_ids[ _tab.selection ] ]->update();
        }
        _ui->pop();
    }

    void Dock::displayContent()
    {
        _quad.color = ( _split ) ? 0x282b3520 : 0x282b3538;

        _ui->push
        ({
            .user_id = _uid_dock[0],
            .wid = _ui->newElem({
                .position = ui::Positioning::Independent,
                .padding = _padding,
                .alignment = {
                    .gap = {6,6}
                },
                .corner_curves = {8,8,8,8},
            }),
            .quad = _quad,
            .clip_overflow = true,
            .name = "dock-body"
        });
        if ( _split )
        {
            if ( _split->update(_ui) )
            {
                _ui->pushLayer(LAYER_DRAGGING_OVERLAY);
                _ui->gui()->beginRoot();
                _ui->gui()->put({ .quad = _split->drag_quad });
                _ui->gui()->endRoot();
                _ui->popLayer();
            }
            _split->nodes[0].displayContent();
            _split->nodes[1].displayContent();
        }
        else if( !_tab.panel_indices.empty() )
        {
            displayTabs();
        }
        _ui->pop();
    }

    void Dock::updateDragAndDrop()
    {
        if( _ui->gui()->isMouseDragging() )
        {
//            _ui->pushLayer(LAYER_DRAGGING_OVERLAY);
//            _ui->gui()->beginRoot();
//            _ui->put
//            ({
//                //.layer = 1,
//                .user_id = _uid_ghost[0],
//                //.wid = _ui->newElem({.wid = _ui->newElem(_ui->theme()->ghost),}),
//                .quad = {
//                    .x = static_cast<float>(_ui->gui()->getInputManager()->getMouse()->getPosition().x),
//                    .y = static_cast<float>(_ui->gui()->getInputManager()->getMouse()->getPosition().y),
//                    .width = 100,
//                    .height = 100,
//                    .color = 0xffffff20,
//                },
//                //.text = _manager->getGhostObject()->dock->_tab.list[ _manager->getGhostObject()->tab_selection ].text,
//            });
//            _ui->gui()->endRoot();
//            _ui->popLayer();
        }

//        if( _manager->getGhostObject()->dragging )
//        {
//            if( !_ui->leftClickDown() )
//            {
//                _manager->getGhostObject()->visible = false;
//                _manager->getGhostObject()->dragging = false;
//                _ui->pushDeferredOp< GhostDropoff >(_uid_ghost[0], _uid, ghostDropoffOp, GhostDropoff{ _manager->getGhostObject(), this });
//            }
//            else if( _ui->pointerDragging() )
//            {
//                _ui->pushDeferredOp<GhostParam>(_uid_ghost[0], _uid, ghostDraggingOp, GhostParam{ _manager->getGhostObject() });
//            }
//        }
    }

    void Dock::update()
    {
        displayContent();
        updateDragAndDrop();
    }

    DockSplit* Dock::split
    (
        float split_ratio,
        ui::Dock::SplitDirection dir,
        const std::vector< std::string >& a_panels,
        const std::vector< std::string >& b_panels
    )
    {
        // Can't split a non-leaf node into a different direction, without first merging
        if (_split)
        {
            return nullptr;
        }
        std::vector< int32_t > a, b;
        for (uint32_t i = 0; i<a_panels.size(); i++)
        {
            int32_t index = _manager->getPanelIndex( a_panels[i] );
            if ( index >= 0 )
            {
                a.push_back( index );
            }
        }
        for (uint32_t i = 0; i<b_panels.size(); i++)
        {
            int32_t index = _manager->getPanelIndex( b_panels[i] );
            if ( index >= 0 )
            {
                b.push_back( index );
            }
        }
        return split( split_ratio, dir, a, b);
    }

    DockSplit* Dock::split(float split_ratio, ui::Dock::SplitDirection dir, const std::vector< int >& a, const std::vector< int >& b)
    {
        if (_split) // Can't split a non-leaf node into a different direction, without first merging
        {
            return nullptr;
        }

        _split = new DockSplit(*this, split_ratio, dir);
        _split->nodes[0]._manager = _manager;
        _split->nodes[1]._manager = _manager;
        _split->nodes[0].setDockPanels( a );
        _split->nodes[1].setDockPanels( b );
        return _split.ref();
    }


    void Dock::onWindowResize(int width, int height)
    {
        if (_parent == nullptr)
        {
            _quad.height = height;
            _quad.width = width;
            _split->onReshape();
        }
    }

    void Dock::merge()
    {
        if ( _split )
        {
            _split->nodes[0].merge();
            _split->nodes[1].merge();
            for (int c=0; c<2; ++c)
            {
                for (int i=0; i<_split->nodes[c]._tab.list.size(); ++i)
                {
                    _tab.list.push_back( _split->nodes[c]._tab.list[i]) ;
                    _tab.panel_indices.push_back( _split->nodes[c]._tab.panel_indices[i] );
                }
            }
            _split = {};
        }
    }

    void Dock::setDockPanels(const std::vector< int >& panel_ids)
    {
        _tab.panel_indices = panel_ids;
        for (int i=0; i<panel_ids.size(); ++i)
        {
            ui::Panel* panel = _manager->getPanel( panel_ids[i] );
            _tab.list.push_back( TabElem({ .text = _ui->gui()->text( panel->_name.c_str(), 20 ) }));
        }
    }

//    void Dock::addPanelToDock(const Ref< ui::Panel >& panel)
//    {
//        _tab.panel_ids.push_back( panel );
//        _tab.list.push_back(TabElem({ .text = _ui->gui()->text(panel->_name.c_str(), 20) }));
//    }

    void swap(ui::Dock& a, ui::Dock& b)
    {
        Tab tab = a._tab;
        a._tab = b._tab;
        b._tab = tab;
    }

    void Dock::ghostDraggingOp(kege::GUI* gui, ui::ID user_id, ui::NodeId node_id, void* data)
    {
        GhostParam* params = reinterpret_cast<GhostParam*>(data);
        if (!params->ghost->visible)
        {
            const NodeId& elem = params->ghost->dock->_tab.list[ params->ghost->tab_selection ].uids[0];
            ui::Node* target = gui->at(elem);
            ui::Node* ghost  = gui->at(node_id);

            params->ghost->visible = true;
            params->ghost->quad.x = target->quad.x;
            params->ghost->quad.y = target->quad.y;
            params->ghost->quad.width = ghost->quad.width;
            params->ghost->quad.height = ghost->quad.height;
        }
        params->ghost->quad.x += gui->getPointerDelta().x;
        params->ghost->quad.y += gui->getPointerDelta().y;
    }

    void Dock::ghostDropoffOp(kege::GUI* gui,  ui::ID user_id, ui::NodeId node_id, void* data)
    {
        GhostDropoff* params = reinterpret_cast<GhostDropoff*>(data);
        params->dock->handleDropOff(*params->dock);
    }

    DropZone Dock::determineDropZone(Dock& target, const kege::dvec2& pointer)
    {
        kege::dvec2 center;
        center.x = target._quad.x + target._quad.width * 0.5f;
        center.y = target._quad.y + target._quad.height * 0.5f;

        kege::dvec2 v = kege::normalize(dvec2{_ui->pointer().x - center.x,  center.y - _ui->pointer().y});
        float tb = kege::dot(v, kege::dvec2(0.f,1.f));
        float lr = kege::dot(v, kege::dvec2(1.f,0.f));

        if (abs(lr) < abs(tb))
        {
            if (0 < tb) // dropoff is on the RIGHT of the node
            {
                return DropZone::TOP;
            }
            else  // dropoff is on the LEFT of the node
            {
                return DropZone::BOTTOM;
            }
        }
        else
        {
            if (0 < lr) // dropoff is on the Top of the node
            {
                return DropZone::RIGHT;
            }
            else // dropoff is on the BOTTOM of the node
            {
                return DropZone::LEFT;
            }
        }
    }

    bool Dock::isSibling(const ui::Dock& a, const ui::Dock& b)
    {
        return a._parent && b._parent && a._parent == b._parent;
    }

    ui::Dock* Dock::findDropTarget(ui::Dock& dock)
    {

        if ( kege::ui::testPointVsRect(_ui->pointer(), dock._quad) )
        {
            if ( dock._split )
            {
                ui::Dock* target = findDropTarget(dock._split->nodes[0]);
                if ( target == nullptr )
                {
                    target = findDropTarget(dock._split->nodes[1]);
                }
                return target;
            }
            return &dock;
        }
        return nullptr;
    }

    void Dock::resolveSiblingDropOff(ui::Dock* target)
    {
        DropZone drop_zone = determineDropZone( *target, _ui->pointer() );
        ui::DockSplit* split = target->_parent->_split.ref();

        if ( &split->nodes[0] == _manager->getGhostObject()->dock )
        {
            if (split->direction == ui::Dock::SplitDirection::VERTICAL)
            {
                switch (drop_zone)
                {
                    case DropZone::LEFT:
                    case DropZone::RIGHT:
                    {
                        split->direction = Dock::SplitDirection::HORIZONTAL;
                        if (drop_zone == DropZone::RIGHT)
                        {
                            swap(*target, *_manager->getGhostObject()->dock);
                        }
                        split->onReshape();
                        break;
                    }
                    case DropZone::BOTTOM:
                    {
                        swap(*target, *_manager->getGhostObject()->dock);
                        break;
                    }
                    default: break;
                }
            }
            else // if (split->direction == AlignDir::HORIZONTAL)
            {
                switch (drop_zone)
                {
                    case DropZone::TOP:
                    case DropZone::BOTTOM:
                    {
                        split->direction = Dock::SplitDirection::VERTICAL;
                        if (drop_zone == DropZone::BOTTOM)
                        {
                            swap(*target, *_manager->getGhostObject()->dock);
                        }
                        split->onReshape();
                        break;
                    }
                    case DropZone::RIGHT:
                    {
                        swap(*target, *_manager->getGhostObject()->dock);
                        break;
                    }
                    default: break;
                }
            }
        }
        else if ( &split->nodes[1] == _manager->getGhostObject()->dock )
        {
            if (split->direction == ui::Dock::SplitDirection::VERTICAL)
            {
                switch (drop_zone)
                {
                    case DropZone::LEFT:
                    case DropZone::RIGHT:
                    {
                        split->direction = Dock::SplitDirection::HORIZONTAL;
                        if (drop_zone == DropZone::LEFT)
                        {
                            swap(*target, *_manager->getGhostObject()->dock);
                        }
                        split->onReshape();
                        //split->onReshape(*target);
                        break;
                    }
                    case DropZone::TOP:
                    {
                        swap(*target, *_manager->getGhostObject()->dock);
                        break;
                    }
                    default: break;
                }
            }
            else // if (split->direction == AlignDir::HORIZONTAL)
            {
                switch (drop_zone)
                {
                    case DropZone::TOP:
                    case DropZone::BOTTOM:
                    {
                        split->direction = Dock::SplitDirection::HORIZONTAL;
                        if (drop_zone == DropZone::TOP)
                        {
                            swap(*target, *_manager->getGhostObject()->dock);
                        }
                        split->onReshape();
                        break;
                    }
                    case DropZone::LEFT:
                    {
                        swap(*target, *_manager->getGhostObject()->dock);
                        break;
                    }
                    default: break;
                }
            }
        }
    }

    void Dock::resolveSelfDropOff(ui::Dock* target)
    {
        GhostObject* ghost = _manager->getGhostObject();
        Dock* ghost_dock = ghost->dock;
        if ( ghost_dock->_tab.panel_indices.size() <= 1 )
        {
            return;
        }

        int pid = target->_tab.panel_indices[ ghost_dock->_tab.selection ];
        Ref< ui::Panel > panel = _manager->getPanel( pid );
        target->_tab.panel_indices.erase(target->_tab.panel_indices.begin() + ghost_dock->_tab.selection);

        DropZone drop_zone = determineDropZone( *target, _ui->pointer() );
        switch (drop_zone)
        {
            case DropZone::TOP:    target->split(0.5, ui::Dock::SplitDirection::VERTICAL,   target->_tab.panel_indices, {panel}); break;
            case DropZone::BOTTOM: target->split(0.5, ui::Dock::SplitDirection::VERTICAL,   {panel}, target->_tab.panel_indices); break;
            case DropZone::LEFT:   target->split(0.5, ui::Dock::SplitDirection::HORIZONTAL, {panel}, target->_tab.panel_indices); break;
            case DropZone::RIGHT:  target->split(0.5, ui::Dock::SplitDirection::HORIZONTAL, target->_tab.panel_indices, {panel}); break;
            default: break;
        }

        ghost_dock->_tab.panel_indices.clear();
        ghost_dock->_tab.list.clear();
        ghost_dock->_tab.selection = 0;

        target->_tab.panel_indices.clear();
        target->_tab.list.clear();
        target->_tab.selection = 0.f;
    }

    void Dock::splitDropOffTarget(ui::Dock* target)
    {
        Dock* ghost_dock = _manager->getGhostObject()->dock;
        DropZone drop_zone = determineDropZone( *target, _ui->pointer() );
        switch (drop_zone)
        {
            case DropZone::LEFT:   target->split(0.5, ui::Dock::SplitDirection::HORIZONTAL, ghost_dock->_tab.panel_indices, target->_tab.panel_indices); break;
            case DropZone::RIGHT:  target->split(0.5, ui::Dock::SplitDirection::HORIZONTAL, target->_tab.panel_indices, ghost_dock->_tab.panel_indices); break;
            case DropZone::TOP:    target->split(0.5, ui::Dock::SplitDirection::VERTICAL,   target->_tab.panel_indices, ghost_dock->_tab.panel_indices); break;
            case DropZone::BOTTOM: target->split(0.5, ui::Dock::SplitDirection::VERTICAL,   ghost_dock->_tab.panel_indices, target->_tab.panel_indices); break;
            default: break;
        }

        Dock* parent = ghost_dock->_parent;
        Dock& survivor = (ghost_dock == &parent->_split->nodes[0])
        ? parent->_split->nodes[1]
        : parent->_split->nodes[0];

        Ref< DockSplit > split = survivor._split;

        if (split) // callapse
        {
            parent->_split = split;
            parent->_split->nodes[0]._parent = parent;
            parent->_split->nodes[1]._parent = parent;
            parent->_split->owner = parent;
            split->onReshape();
        }

        target->_tab.panel_indices.clear();
        target->_tab.list.clear();
        target->_tab.selection = 0.f;

        ghost_dock->_tab.panel_indices.clear();
        ghost_dock->_tab.list.clear();
        ghost_dock->_tab.selection = 0;
    }

    void Dock::resolveTabDropOff(ui::Dock* target)
    {
        Dock* ghost_dock = _manager->getGhostObject()->dock;
        /**
         check if the drop spot already contain the panel being dropped off.
         if it has that panel exit
         */
        size_t min_count = kege::min(target->_tab.panel_indices.size(), ghost_dock->_tab.panel_indices.size());
        for(int i=0; i<min_count; ++i)
        {
            Panel* panel = _manager->getPanel( ghost_dock->_tab.panel_indices[i] );
            Panel* target_panel = _manager->getPanel( target->_tab.panel_indices[i] );
            if ( panel->getName() == target_panel->getName() )
            {
                return;
            }
        }

        /**
         if the drop off target and the origin has the same parent merge them
         */
        if ( target->_parent == ghost_dock->_parent && target->_parent != nullptr && ghost_dock->_tab.panel_indices.size() == 1 )
        {
            target->_parent->merge();
            return;
        }
        /**
         otherwise add panel to dropoff target tablist
         */
        target->_tab.list.push_back(TabElem({ .text = ghost_dock->_tab.list[ _manager->getGhostObject()->tab_selection ].text }));
        target->_tab.panel_indices.push_back( ghost_dock->_tab.panel_indices[ ghost_dock->_tab.selection ] );
        /**
         remove panel from old tablist
         */
        ghost_dock->_tab.panel_indices.erase( ghost_dock->_tab.panel_indices.begin() + ghost_dock->_tab.selection );
        ghost_dock->_tab.list.erase( ghost_dock->_tab.list.begin() + ghost_dock->_tab.selection );

        /**
         if old tablist is empty then old dock parent shold merge into one
         */
        if ( ghost_dock->_tab.list.empty() )
        {
            Dock* parent = ghost_dock->_parent;

            Dock& survivor = (ghost_dock == &parent->_split->nodes[0])
            ? parent->_split->nodes[1]
            : parent->_split->nodes[0];

            Ref< DockSplit > split = survivor._split;

            if (split) // callapse
            {
                parent->_split = split;
                parent->_split->nodes[0]._parent = parent;
                parent->_split->nodes[1]._parent = parent;
                parent->_split->owner = parent;
                split->onReshape();
            }
            else
            {
                parent->_tab.panel_indices = std::move( survivor._tab.panel_indices );
                parent->_tab.list = std::move( survivor._tab.list );
                parent->_name = survivor._name;
                parent->_split = {};
            }
        }
        else
        {
            ghost_dock->_tab.selection = ghost_dock->_tab.selection % int(ghost_dock->_tab.list.size());
        }
    }

    bool Dock::handleDropOff(ui::Dock& dock)
    {
        ui::Dock* target = findDropTarget( dock );
        if ( target == nullptr ) {
            return false;
        }

        Dock* ghost_dock = _manager->getGhostObject()->dock;
        if ( target == ghost_dock )
        {
            resolveSelfDropOff(target);
            return true;
        }

        const Quad& tab_area = _ui->get( target->_tab.node_id )->quad;
        if ( kege::ui::testPointVsRect(_ui->pointer(), tab_area) )
        {
            resolveTabDropOff( target );
            return true;
        }

        if ( isSibling( *target, *ghost_dock ) )
        {
            resolveSiblingDropOff( target );
            return true;
        }

        splitDropOffTarget(target);
        return true;
    }

    Dock::Dock(kege::ui::DockManager* manager, const kege::ui::Quad& quad)
    :   _manager( manager )
    ,   _ui( manager->getUI() )
    ,   _quad( quad )
    {
        _padding = {10,10,10,10};
        _parent = nullptr;
        
//        _uid_ghost;
//        _uid_dock;
//        _uid_tab;
    }

    Dock::Dock()
    :   _manager(nullptr)
    ,   _parent(nullptr)
    ,   _padding{0,0,0,0}
    ,   _ui(nullptr)
    {
    }

    Dock::~Dock()
    {
        _split.clear();
    }
}






