//
//  ui-docking-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#include "ui-dock.hpp"

namespace kege::ui{

    DockSplit* Dock::split(ui::AlignDir dir, const std::vector< int >& a, const std::vector< int >& b)
    {
        // Can't split a non-leaf node into a different direction, without first merging
        if (_split)
        {
            return nullptr;
        }

        _split = new DockSplit(*this, 0.5, dir);
        _split->nodes[0]._context = _context;
        _split->nodes[1]._context = _context;
        _split->nodes[0].setDockPanels( a );
        _split->nodes[1].setDockPanels( b );
        return _split.ref();
    }


    void Dock::onWindowResize(int width, int height)
    {
        if (_parent == nullptr)
        {
            _rect.height = height;
            _rect.width = width;
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
                    _tab.panel_ids.push_back( _split->nodes[c]._tab.panel_ids[i] );
                }
            }
            _split = {};
        }
    }

    void Dock::setDockPanels(const std::vector< int >& panel_ids)
    {
        _tab.panel_ids = panel_ids;
        for (int i=0; i<panel_ids.size(); ++i)
        {
            Ref<ui::Panel >& panel = _context->panels[ panel_ids[i] ];
            _tab.list.push_back( TabElem({ .text = _context->gui->layout()->text( panel->_name.c_str(), 20 ) }));
        }
    }

//    void Dock::addPanelToDock(const Ref< ui::Panel >& panel)
//    {
//        _tab.panel_ids.push_back( panel );
//        _tab.list.push_back(TabElem({ .text = _context->gui->layout()->text(panel->_name.c_str(), 20) }));
//    }

    void swap(ui::Dock& a, ui::Dock& b)
    {
        Tab tab = a._tab;
        a._tab = b._tab;
        b._tab = tab;
    }

    void Dock::ghostDraggingOp(ui::Layout* layout, const ui::UID* id, void* data)
    {
        GhostParam* params = reinterpret_cast<GhostParam*>(data);
        if (!params->ghost->visible)
        {
            const UID& elem = params->ghost->dock->_tab.list[ params->ghost->tab_selection ].uids[0];
            Widget* target = layout->elem(elem);
            Widget* ghost  = layout->elem(*id);

            params->ghost->visible = true;
            params->ghost->rect.x = target->rect.x;
            params->ghost->rect.y = target->rect.y;
            params->ghost->rect.width = ghost->rect.width;
            params->ghost->rect.height = ghost->rect.height;
        }
        params->ghost->rect.x += layout->getPointerDelta().x;
        params->ghost->rect.y += layout->getPointerDelta().y;
    }

    void Dock::ghostDropoffOp(ui::Layout* layout, const ui::UID* id, void* data)
    {
        GhostDropoff* params = reinterpret_cast<GhostDropoff*>(data);
        params->dock->handleDropOff(*params->dock);
    }

    void Dock::updateTabs()
    {
        _context->gui->push({.layer = 0, .style = &_context->style_dock});
        {
            _context->gui->push
            ({
                .layer = 0,
                .uid = &_tab.uid,
                .style = &_context->style_tab
            });
            for (int i=0; i<_tab.list.size(); ++i)
            {
                TabElem& elem = _tab.list[i];

                _context->gui->push
                ({
                    .layer = 0,
                    .style = &_context->style_tab_elem,
                    .uid = &elem.uids[0],
                    .single_click = ui::ClickTrigger::Continuous,
                    .border.corner_curves = {8,8,0,0},
                });
                _context->gui->put
                ({
                    .layer = 0,
                    .style = &_context->style_tab_label,
                    .uid = &elem.uids[1],
                    .text = elem.text,
                    .mouseover = false,
                });
                _context->gui->put
                ({
                    .layer = 0,
                    .uid = &elem.uids[2],
                    .rect.height = 10,
                    .rect.width = 10,
                    .rect.y = 4,
                    .color = _context->gui->mouseover( elem.uids[2] ) ? 0xEE2200FF : 0xEE220050,
                    .single_click = ui::ClickTrigger::OnRelease,
                    .double_click = ui::ClickTrigger::Immediate,
                    .border.corner_curves = {8,8,8,8}
                });
                _context->gui->pop();

                if( _context->gui->click( elem.uids[0] ) )
                {
                    _tab.selection = i;
                    if (!_context->ghost.dragging && _context->gui->pointerDragging())
                    {
                        _context->ghost.dragging = true;
                        _context->ghost.dock = this;
                        _context->ghost.tab_selection = i;
                    }
                }
            }
            _context->gui->pop();

            _context->panels[ _tab.panel_ids[ _tab.selection ] ]->update();
        }
        _context->gui->pop();
    }

    void Dock::updateContent()
    {
        _context->gui->push
        ({
            .layer = 0,
            .uid = &_uid,
            .rect = _rect,
            .position = ui::Positioning::Independent,
            .color = 0xD2A84208,
            .padding = _padding,
            .clip_overflow = true,
        });
        if ( _split )
        {
            if ( _split->update(_context->gui) )
            {
                _context->gui->layout()->putRoot
                ({
                    .layer = 1,
                    .rect = _split->drag_rect,
                    .color = 0xFFFFFF30,
                });
            }

            _split->nodes[0].updateContent();
            _split->nodes[1].updateContent();
        }
        else if( !_tab.panel_ids.empty() )
        {
            updateTabs();
        }
        _context->gui->pop();
    }

    void Dock::updateDragAndDrop()
    {
        if( _context->ghost.visible )
        {
            _context->gui->put
            ({
                .layer = 1,
                .uid = &_uid,
                .text = _context->ghost.dock->_tab.list[ _context->ghost.tab_selection ].text,
                .rect.x = _context->ghost.rect.x,
                .rect.y = _context->ghost.rect.y,
                .border.corner_curves = {8,8,8,8},
                .style = &_context->style_ghost,
            });
        }
        if( _context->ghost.dragging )
        {
            if( !_context->gui->buttonDown() )
            {
                _context->ghost.visible = false;
                _context->ghost.dragging = false;
                _context->gui->pushDeferredOp< GhostDropoff >(&_uid, ghostDropoffOp, GhostDropoff{ &_context->ghost, this });
            }
            else if( _context->gui->pointerDragging() )
            {
                _context->gui->pushDeferredOp<GhostParam>(&_uid, ghostDraggingOp, GhostParam{ &_context->ghost });
            }
        }
    }
    
    void Dock::update()
    {
        updateContent();
        updateDragAndDrop();
    }

    DropZone Dock::determineDropZone(Dock& target, const kege::dvec2& pointer)
    {
        kege::dvec2 center;
        center.x = target._rect.x + target._rect.width * 0.5f;
        center.y = target._rect.y + target._rect.height * 0.5f;

        kege::dvec2 v = kege::normalize(dvec2{_context->gui->pointer().x - center.x,  center.y - _context->gui->pointer().y});
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

        if ( _context->gui->layout()->testPointVsRect(_context->gui->pointer(), dock._rect) )
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
        DropZone drop_zone = determineDropZone( *target, _context->gui->pointer() );
        ui::DockSplit* split = target->_parent->_split.ref();

        if ( &split->nodes[0] == _context->ghost.dock )
        {
            if (split->direction == ui::AlignDir::VERTICAL)
            {
                switch (drop_zone)
                {
                    case DropZone::LEFT:
                    case DropZone::RIGHT:
                    {
                        split->direction = AlignDir::HORIZONTAL;
                        if (drop_zone == DropZone::RIGHT)
                        {
                            swap(*target, *_context->ghost.dock);
                        }
                        split->onReshape();
                        break;
                    }
                    case DropZone::BOTTOM:
                    {
                        swap(*target, *_context->ghost.dock);
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
                        split->direction = AlignDir::VERTICAL;
                        if (drop_zone == DropZone::BOTTOM)
                        {
                            swap(*target, *_context->ghost.dock);
                        }
                        split->onReshape();
                        break;
                    }
                    case DropZone::RIGHT:
                    {
                        swap(*target, *_context->ghost.dock);
                        break;
                    }
                    default: break;
                }
            }
        }
        else if ( &split->nodes[1] == _context->ghost.dock )
        {
            if (split->direction == ui::AlignDir::VERTICAL)
            {
                switch (drop_zone)
                {
                    case DropZone::LEFT:
                    case DropZone::RIGHT:
                    {
                        split->direction = AlignDir::HORIZONTAL;
                        if (drop_zone == DropZone::LEFT)
                        {
                            swap(*target, *_context->ghost.dock);
                        }
                        split->onReshape();
                        //split->onReshape(*target);
                        break;
                    }
                    case DropZone::TOP:
                    {
                        swap(*target, *_context->ghost.dock);
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
                        split->direction = AlignDir::VERTICAL;
                        if (drop_zone == DropZone::TOP)
                        {
                            swap(*target, *_context->ghost.dock);
                        }
                        split->onReshape();
                        break;
                    }
                    case DropZone::LEFT:
                    {
                        swap(*target, *_context->ghost.dock);
                        break;
                    }
                    default: break;
                }
            }
        }
    }

    void Dock::resolveSelfDropOff(ui::Dock* target)
    {
        if ( _context->ghost.dock->_tab.panel_ids.size() <= 1 )
        {
            return;
        }

        int pid = target->_tab.panel_ids[ _context->ghost.dock->_tab.selection ];
        Ref< ui::Panel > panel = _context->panels[ pid ];
        target->_tab.panel_ids.erase(target->_tab.panel_ids.begin() + _context->ghost.dock->_tab.selection);

        DropZone drop_zone = determineDropZone( *target, _context->gui->pointer() );
        switch (drop_zone)
        {
            case DropZone::TOP:    target->split(ui::AlignDir::VERTICAL,   target->_tab.panel_ids, {panel}); break;
            case DropZone::BOTTOM: target->split(ui::AlignDir::VERTICAL,   {panel}, target->_tab.panel_ids); break;
            case DropZone::LEFT:   target->split(ui::AlignDir::HORIZONTAL, {panel}, target->_tab.panel_ids); break;
            case DropZone::RIGHT:  target->split(ui::AlignDir::HORIZONTAL, target->_tab.panel_ids, {panel}); break;
            default: break;
        }

        _context->ghost.dock->_tab.panel_ids.clear();
        _context->ghost.dock->_tab.list.clear();
        _context->ghost.dock->_tab.selection = 0;

        target->_tab.panel_ids.clear();
        target->_tab.list.clear();
        target->_tab.selection = 0.f;
    }

    void Dock::splitDropOffTarget(ui::Dock* target)
    {
        DropZone drop_zone = determineDropZone( *target, _context->gui->pointer() );
        switch (drop_zone)
        {
            case DropZone::LEFT:   target->split(ui::AlignDir::HORIZONTAL, _context->ghost.dock->_tab.panel_ids, target->_tab.panel_ids); break;
            case DropZone::RIGHT:  target->split(ui::AlignDir::HORIZONTAL, target->_tab.panel_ids, _context->ghost.dock->_tab.panel_ids); break;
            case DropZone::TOP:    target->split(ui::AlignDir::VERTICAL,   target->_tab.panel_ids, _context->ghost.dock->_tab.panel_ids); break;
            case DropZone::BOTTOM: target->split(ui::AlignDir::VERTICAL,   _context->ghost.dock->_tab.panel_ids, target->_tab.panel_ids); break;
            default: break;
        }

        Dock* parent = _context->ghost.dock->_parent;
        Dock& survivor = (_context->ghost.dock == &parent->_split->nodes[0])
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

        target->_tab.panel_ids.clear();
        target->_tab.list.clear();
        target->_tab.selection = 0.f;

        _context->ghost.dock->_tab.panel_ids.clear();
        _context->ghost.dock->_tab.list.clear();
        _context->ghost.dock->_tab.selection = 0;
    }

    void Dock::resolveTabDropOff(ui::Dock* target)
    {
        /**
         check if the drop spot already contain the panel being dropped off.
         if it has that panel exit
         */
        size_t min_count = kege::min(target->_tab.panel_ids.size(), _context->ghost.dock->_tab.panel_ids.size());
        for(int i=0; i<min_count; ++i)
        {
            Ref< Panel >& panel = _context->panels[ _context->ghost.dock->_tab.panel_ids[i] ];
            if ( panel->getName() == _context->panels[ target->_tab.panel_ids[i] ]->getName() )
            {
                return;
            }
        }

        /**
         if the drop off target and the origin has the same parent merge them
         */
        if ( target->_parent == _context->ghost.dock->_parent && target->_parent != nullptr && _context->ghost.dock->_tab.panel_ids.size() == 1 )
        {
            target->_parent->merge();
            return;
        }
        /**
         otherwise add panel to dropoff target tablist
         */
        target->_tab.list.push_back(TabElem({ .text = _context->ghost.dock->_tab.list[ _context->ghost.tab_selection ].text }));
        target->_tab.panel_ids.push_back( _context->ghost.dock->_tab.panel_ids[ _context->ghost.dock->_tab.selection ] );
        /**
         remove panel from old tablist
         */
        _context->ghost.dock->_tab.panel_ids.erase( _context->ghost.dock->_tab.panel_ids.begin() + _context->ghost.dock->_tab.selection );
        _context->ghost.dock->_tab.list.erase( _context->ghost.dock->_tab.list.begin() + _context->ghost.dock->_tab.selection );

        /**
         if old tablist is empty then old dock parent shold merge into one
         */
        if ( _context->ghost.dock->_tab.list.empty() )
        {
            Dock* parent = _context->ghost.dock->_parent;

            Dock& survivor = (_context->ghost.dock == &parent->_split->nodes[0])
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
                parent->_tab.panel_ids = std::move( survivor._tab.panel_ids );
                parent->_tab.list = std::move( survivor._tab.list );
                parent->_name = survivor._name;
                parent->_split = {};
            }
        }
        else
        {
            _context->ghost.dock->_tab.selection = _context->ghost.dock->_tab.selection % int(_context->ghost.dock->_tab.list.size());
        }
    }

    bool Dock::handleDropOff(ui::Dock& dock)
    {
        ui::Dock* target = findDropTarget( dock );
        if ( target == nullptr ) {
            return false;
        }

        if ( target == _context->ghost.dock )
        {
            resolveSelfDropOff(target);
            return true;
        }

        const Rect& tab_area = _context->gui->get( target->_tab.uid )->rect;
        if ( _context->gui->layout()->testPointVsRect(_context->gui->pointer(), tab_area) )
        {
            resolveTabDropOff( target );
            return true;
        }

        if ( isSibling( *target, *_context->ghost.dock ) )
        {
            resolveSiblingDropOff( target );
            return true;
        }

        splitDropOffTarget(target);
        return true;
    }

    Dock::Dock(ui::DockContext* context, int width, int height)
    :   _context( context )
    {
        _padding = {6,6,6,6};
        _parent = nullptr;
        _rect.height = height;
        _rect.width = width;
        _rect.x = 0.f;
        _rect.y = 0.f;
    }

    Dock::Dock()
    :   _parent(nullptr)
    ,   _padding{0,0,0,0}
    {}

    Dock::~Dock()
    {
        _split.clear();
    }
}








namespace kege::ui{

    DockSplit::DockSplit(Dock& dock, float slit_ratio, ui::AlignDir direction)
    :   slit_ratio( slit_ratio )
    ,   direction( direction )
    ,   gap( 6 )
    ,   owner(&dock)
    {
        nodes[0]._parent = &dock;
        nodes[1]._parent = &dock;
        onReshape();
    }

    void DockSplit::updateHorizontalDragRect()
    {
        drag_rect.x = nodes[0]._rect.x + nodes[0]._rect.width;
        drag_rect.y = nodes[0]._rect.y;
        drag_rect.width = nodes[1]._rect.x - drag_rect.x;
        drag_rect.height = nodes[0]._rect.height;
    }

    void DockSplit::updateVerticalDragRect()
    {
        drag_rect.y = nodes[0]._rect.y + nodes[0]._rect.height;
        drag_rect.x = nodes[0]._rect.x;
        drag_rect.width = nodes[0]._rect.width;
        drag_rect.height = nodes[1]._rect.y - drag_rect.y;
    }

    void DockSplit::onReshape()
    {
        if (this->direction == ui::AlignDir::VERTICAL)
        {
            float height = owner->_rect.height - (gap + owner->_padding.above + owner->_padding.below);

            nodes[0]._rect.height = height * slit_ratio;
            nodes[0]._rect.width  = owner->_rect.width - (owner->_padding.left + owner->_padding.above);
            nodes[0]._rect.x      = owner->_rect.x + owner->_padding.left;
            nodes[0]._rect.y      = owner->_rect.y + owner->_padding.above;

            nodes[1]._rect.height = height - nodes[0]._rect.height;
            nodes[1]._rect.width  = nodes[0]._rect.width;
            nodes[1]._rect.x      = nodes[0]._rect.x;
            nodes[1]._rect.y      = nodes[0]._rect.y + nodes[0]._rect.height + gap;

            updateVerticalDragRect();
        }
        else
        {
            float width = owner->_rect.width - (gap + owner->_padding.left + owner->_padding.right);

            nodes[0]._rect.width  = width * slit_ratio;
            nodes[0]._rect.height = owner->_rect.height - (owner->_padding.above + owner->_padding.below);
            nodes[0]._rect.x      = owner->_rect.x + owner->_padding.left;
            nodes[0]._rect.y      = owner->_rect.y + owner->_padding.above;

            nodes[1]._rect.width  = width - nodes[0]._rect.width;
            nodes[1]._rect.height = nodes[0]._rect.height;
            nodes[1]._rect.x      = nodes[0]._rect.x + nodes[0]._rect.width + gap;
            nodes[1]._rect.y      = nodes[0]._rect.y;

            updateHorizontalDragRect();
        }
        if ( nodes[0]._split ) nodes[0]._split->onReshape();
        if ( nodes[1]._split ) nodes[1]._split->onReshape();
    }

    void DockSplit::onResizeGrandChild(ui::AlignDir align_direction, const Rect& drag_rect_resizer)
    {
        float closest_1, closest_2, length;

        if (align_direction == ui::AlignDir::VERTICAL && direction == ui::AlignDir::VERTICAL)
        {
            closest_1 = drag_rect_resizer.y - nodes[0]._rect.y;
            closest_2 = drag_rect_resizer.y - nodes[1]._rect.y;
            if (closest_1 < closest_2)
            {
                nodes[0]._rect.y = owner->_rect.y + owner->_padding.above;
                nodes[0]._rect.height = owner->_rect.height - nodes[1]._rect.height - gap - owner->_padding.above;
                length = nodes[1]._rect.height;

                if ( nodes[0]._split ) nodes[0]._split->onResize(align_direction, drag_rect_resizer);
            }
            else
            {
                nodes[1]._rect.y = nodes[0]._rect.y + gap + nodes[0]._rect.height;
                nodes[1]._rect.height = owner->_rect.height - nodes[0]._rect.height - gap - owner->_padding.below;
                length = nodes[0]._rect.height;

                if ( nodes[1]._split ) nodes[1]._split->onResize(align_direction, drag_rect_resizer);
            }
            slit_ratio = length / (owner->_rect.height - owner->_padding.above - owner->_padding.below - gap);
        }

        else if (align_direction == ui::AlignDir::HORIZONTAL && direction == ui::AlignDir::HORIZONTAL)
        {
            closest_1 = drag_rect_resizer.x - nodes[0]._rect.x;
            closest_2 = drag_rect_resizer.x - nodes[1]._rect.x;

            if (closest_1 < closest_2)
            {
                nodes[0]._rect.x = owner->_rect.x + owner->_padding.left;
                nodes[0]._rect.width = owner->_rect.width - nodes[1]._rect.width - gap - owner->_padding.left;
                length = nodes[1]._rect.width;

                if ( nodes[0]._split ) nodes[0]._split->onResize(align_direction, drag_rect_resizer);
            }
            else
            {
                nodes[1]._rect.x = nodes[0]._rect.x + nodes[0]._rect.width + gap;
                nodes[1]._rect.width = owner->_rect.width - nodes[0]._rect.width - gap - owner->_padding.right;
                length = nodes[0]._rect.width;

                if ( nodes[1]._split ) nodes[1]._split->onResize(align_direction, drag_rect_resizer);
            }
            slit_ratio = length / (owner->_rect.width - owner->_padding.left - owner->_padding.right - gap);
        }

        else
        {
            onResize(align_direction, drag_rect_resizer);
        }
    }

    void DockSplit::onResize(ui::AlignDir align_direction, const Rect& drag_rect)
    {
        if (direction == ui::AlignDir::VERTICAL)
        {
            float height = owner->_rect.height
                         - gap
                         - owner->_padding.above
                         - owner->_padding.below;

            float h0 = height * slit_ratio;
            float h1 = height - h0;

            nodes[0]._rect = {
                owner->_rect.x + owner->_padding.left,
                owner->_rect.y + owner->_padding.above,
                owner->_rect.width - owner->_padding.left - owner->_padding.right,
                h0
            };

            nodes[1]._rect = {
                nodes[0]._rect.x,
                nodes[0]._rect.y + h0 + gap,
                nodes[0]._rect.width,
                h1
            };

            updateVerticalDragRect();
        }
        else
        {
            float width = owner->_rect.width
                        - gap
                        - owner->_padding.left
                        - owner->_padding.right;

            float w0 = width * slit_ratio;
            float w1 = width - w0;

            nodes[0]._rect = {
                owner->_rect.x + owner->_padding.left,
                owner->_rect.y + owner->_padding.above,
                w0,
                owner->_rect.height - owner->_padding.above - owner->_padding.below
            };

            nodes[1]._rect = {
                nodes[0]._rect.x + w0 + gap,
                nodes[0]._rect.y,
                w1,
                nodes[0]._rect.height
            };

            updateHorizontalDragRect();
        }

        if (nodes[0]._split)
        {
            nodes[0]._split->onResizeGrandChild(align_direction, drag_rect);
        }
        if (nodes[1]._split)
        {
            nodes[1]._split->onResizeGrandChild(align_direction, drag_rect);
        }
    }

    bool DockSplit::update(GUI* gui)
    {
        if (!gui->buttonDown())
            drag_resize = gui->testPointVsRect(gui->pointer(), drag_rect);

        else if (drag_resize)
        {
            if( gui->pointerDragging() )
            {
                float amount, available, delta_ratio;
                if( direction == AlignDir::VERTICAL )
                {
                    amount = gui->deltaPointer().y;
                    available = owner->_rect.height - gap - owner->_padding.above - owner->_padding.below;
                    delta_ratio = amount / available;
                }
                else
                {
                    amount = gui->deltaPointer().x;
                    available = owner->_rect.width - gap - owner->_padding.left - owner->_padding.right;
                    delta_ratio = amount / available;
                }
                slit_ratio = clamp(slit_ratio + delta_ratio, 0.05f, 0.95f);

                onResize(direction, drag_rect);
            }
        }
        return drag_resize;
    }

    DockSplit::~DockSplit()
    {
    }
}
