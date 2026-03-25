//
//  drag-drop-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/16/26.
//

#ifndef drag_drop_manager_hpp
#define drag_drop_manager_hpp

#include "ui-panel.hpp"

namespace kege::ui{

    class DragDropManager
    {
    private:
//        std::unique_ptr<DragData> _currentDragData;
//        DropTarget* _currentHoverTarget = nullptr;
//        DropTarget* _dragSource = nullptr;
//        kege::dvec2 _dragStartPos;
//        bool _isDragging = false;
//        float _dragThreshold = 5.0f; // pixels before drag starts
//
    public:
//        enum class DragState { Inactive, PossibleDrag, Dragging };
//
//        bool onMouseDown(const kege::dvec2& mousePos, DragSource* source) {
//            if (_isDragging) return false;
//
//            _dragSource = source;
//            _dragStartPos = mousePos;
//            _state = DragState::PossibleDrag;
//            return true;
//        }
//
//        void onMouseMove(const kege::dvec2& mousePos, const std::vector<DropTarget*>& targets) {
//            if (_state == DragState::PossibleDrag) {
//                if (kege::distance(mousePos, _dragStartPos) > _dragThreshold) {
//                    startDrag();
//                }
//            }
//
//            if (_isDragging) {
//                updateHover(mousePos, targets);
//            }
//        }
//
//        void onMouseUp(const kege::dvec2& mousePos) {
//            if (_isDragging) {
//                if (_currentHoverTarget) {
//                    _currentHoverTarget->onDrop(_currentDragData.get(), mousePos);
//                }
//                endDrag();
//            } else {
//                _state = DragState::Inactive;
//            }
//        }
//
//    private:
//        void startDrag() {
//            _currentDragData = _dragSource->startDrag(_dragStartPos);
//            if (_currentDragData) {
//                _isDragging = true;
//                _state = DragState::Dragging;
//            }
//        }
//
//        void updateHover(const kege::dvec2& mousePos, const std::vector<DropTarget*>& targets) {
//            auto* newTarget = findDropTarget(mousePos, targets);
//
//            if (newTarget != _currentHoverTarget) {
//                if (_currentHoverTarget) {
//                    _currentHoverTarget->onDragLeave();
//                }
//                _currentHoverTarget = newTarget;
//            }
//        }
    };
}

#endif /* drag_drop_manager_hpp */
