//
//  drag-drop-controller.cpp
//  editor
//
//  Created by Kenneth Esdaile on 8/1/26.
//

#include "selection-controller.hpp"

namespace kege::ui{

//    void SelectionController::selectable(ID id, bool left_released, bool right_released)
//    {
//        bool cmd = _input->getKeyboard()->isDown(KEY_LEFT_CONTROL) ||
//                   _input->getKeyboard()->isDown(KEY_RIGHT_CONTROL);
//
//        if (cmd && right_released)
//        {
//            // ctrl/cmd + right click: remove from selection
//            auto it = std::find(_selected.begin(), _selected.end(), id);
//            if (it != _selected.end()) _selected.erase(it);
//            return;
//        }
//
//        if (left_released)
//        {
//            if (cmd)
//            {
//                // ctrl/cmd + click: add to selection if not already present
//                if (std::find(_selected.begin(), _selected.end(), id) == _selected.end())
//                    _selected.push_back(id);
//            }
//            else
//            {
//                // plain click: exclusive select
//                _selected.clear();
//                _selected.push_back(id);
//            }
//        }
//    }

//    void SelectionController::draggable(ID id, bool hovered, const Rect& rect, int drag_type)
//    {
//        auto* mouse = _input->getMouse();
//
//        if (_phase == DragPhase::Idle && hovered && mouse->isPressed(kege::MouseButtonCode::Middle))
//        {
//            _phase = DragPhase::Pending;
//            _armed_id = id;
//            _armed_rect = rect;
//            _armed_type = drag_type;
//            _press_pos = mouse->getPosition();
//        }
//    }

    void SelectionController::updateMarqueeRectangule()
    {
        if( _gui->leftClickDown() )
        {
            if( !_gui->hasHit() && _phase == DragPhase::Idle )
            {
                _press_pos.x = _input->getMouse()->getPosition().x;
                _press_pos.y = _input->getMouse()->getPosition().y;
                _phase = DragPhase::Marqueeing;
            }
            else if( _phase == DragPhase::Marqueeing )
            {
                _marquee_rect.x = min(_press_pos.x, _input->getMouse()->getPosition().x);
                _marquee_rect.y = min(_press_pos.y, _input->getMouse()->getPosition().y);
                _marquee_rect.width = abs(_press_pos.x - _input->getMouse()->getPosition().x);
                _marquee_rect.height = abs(_press_pos.y - _input->getMouse()->getPosition().y);
            }
        }
        else if( !_gui->leftClickDown() && _phase == DragPhase::Marqueeing )
        {
            _phase = DragPhase::Idle;
            _marquee_rect = {};
        }
    }

    void SelectionController::updateSelectionList()
    {
        if( _gui->leftClickDown() && !_button_down )
        {
            _button_down = true;
            if( _gui->hasHit() )
            {
                auto it = _selected.find(_gui->getHitId());
                Modifiers mods = Modifiers::Shift | Modifiers::Control;
                // if item is not in list add it
                if ( it == _selected.end() )
                {
                    if ( contain(_input->getMouse()->getModifiers(), mods) )
                    {
                        _selected.insert( _gui->getHitId() );
                    }
                    else if( _selected.empty() )
                    {
                        _selected.insert( _gui->getHitId() );
                    }
                }
                // else if item already exist and shift is down them remove item
                else if( contain(_input->getMouse()->getModifiers(), mods) )
                {
                    _selected.erase(it);
                }
            }
            else
            {
                _selected.clear();
            }
        }
        else if ( !_gui->leftClickDown() && _button_down )
        {
            _button_down = false;
        }
    }

    void SelectionController::updateDragging()
    {
        auto* mouse = _input->getMouse();

        switch (_phase)
        {
            case kege::ui::DragPhase::Pending:
            {
                if (!mouse->isDown(kege::MouseButtonCode::Left)) { _phase = kege::ui::DragPhase::Idle; break; }

                if (kege::magn(mouse->getPosition() - _press_pos) > _drag_threshold)
                {
                    _phase = kege::ui::DragPhase::Dragging;

                    // if the armed item isn't part of the current selection,
                    // dragging it selects it exclusively (standard behavior)
                    if (_selected.find(_armed_id) == _selected.end())
                    {
                        _selected.clear();
                        _selected.insert(_armed_id);
                    }

                    _payload.ids = _selected;
                    _payload.type = _armed_type;
                }
                break;
            }

            case kege::ui::DragPhase::Dragging:
            {
                if (!mouse->isDown(kege::MouseButtonCode::Left))
                {
                    // released — find topmost accepting zone under the cursor
                    kege::dvec2 p = mouse->getPosition();
                    _dropped_target = ID{};
                    for (auto it = _zones_this_frame.rbegin(); it != _zones_this_frame.rend(); ++it)
                    {
                        if (it->accept_type == _payload.type && kege::ui::testPointVsRect(it->rect, p))
                        {
                            _dropped_target = it->id;
                            _drop_pending = true;
                            break;
                        }
                    }
                    _phase = DragPhase::Idle;
                }
                break;
            }

            default: break;
        }

        _zones_this_frame.clear(); // rebuilt fresh next frame
    }

    void SelectionController::drawMarquee()
    {
        if ( _phase == DragPhase::Marqueeing )
        {
            _gui->pushLayer(LAYER_BASE_OVERLAY);
            _gui->beginRoot();
            _gui->put
            ({
                .quad =
                {
                    .x = _marquee_rect.x,
                    .y = _marquee_rect.y,
                    .width = _marquee_rect.width,
                    .height = _marquee_rect.height,
                    .color = 0xFFFFFF10
                }
            });
            _gui->endRoot();
            _gui->popLayer();
        }
    }

    void SelectionController::update()
    {
        updateSelectionList();
        updateDragging();
        //drawMarquee();

        for (auto it = _selected.begin(); it != _selected.end(); ++it)
        {
            //_gui->at()->quad.color = 0xFFFFFFF0;
        }
    }

    void SelectionController::registerDropTarget(ID id, int type, void* payload)
    {
        //_zones_this_frame.push_back({id, rect, accept_type});
    }

//    bool SelectionController::consumeDrop(ID target_id, DragPayload& out_payload)
//    {
//        if (_drop_pending && _dropped_target == target_id)
//        {
//            out_payload = _payload;
//            _drop_pending = false;
//            return true;
//        }
//        return false;
//    }

//    bool UI::selectableItem(ui::ID user_id, ui::Rect rect, const char* label)
//    {
//        _gui->push({
//            .user_id = user_id,
//            .wid = _gui->newElem({ _theme->list_item }),
//            .single_click = ui::ClickTrigger::OnRelease,
//        });
//        _gui->put({ .text = { .data = label } });
//        _gui->pop();
//
//        bool hovered  = _gui->isHovered(user_id);
//        bool released = _gui->click(user_id);
//        bool r_released = _gui->rightClick(user_id); // add if you don't have this yet
//
//        _dragdrop.selectable(user_id, released, r_released);
//        _dragdrop.draggable(user_id, hovered, rect, DRAG_TYPE_LIST_ITEM);
//
//        return _dragdrop.isSelected(user_id);
//    }

//    void Docking::buildZone(ui::ID zone_id, ui::Rect rect)
//    {
//        // ... existing dock-zone UI ...
//        _dragdrop.registerDropTarget(zone_id, rect, DRAG_TYPE_WINDOW);
//    }
//
//    void Docking::processDrops()
//    {
//        for (auto& zone : _zones)
//        {
//            ui::DragPayload payload;
//            if (_dragdrop.consumeDrop(zone.id, payload))
//            {
//                // docking-specific: pick split direction from where in the zone
//                // the drop happened, then splice payload.ids into the dock tree
//                insertIntoDockTree(zone, payload.ids);
//            }
//        }
//    }

    bool SelectionController::isSelected(kege::ui::ID id) const
    {
        return _selected.find( id ) != _selected.end();
    }

    SelectionController::SelectionController(kege::GUI* gui)
    :   _input( gui->getInputManager() )
    ,   _gui( gui )
    {}
}
