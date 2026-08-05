//
//  drag-drop-controller.hpp
//  editor
//
//  Created by Kenneth Esdaile on 8/1/26.
//

#ifndef drag_drop_controller_hpp
#define drag_drop_controller_hpp

#include "../common/ui-description.hpp"

namespace kege::ui{

    enum class DragPhase { Idle, Pending, Dragging, Marqueeing };

    struct DragPayload
    {
        std::unordered_set<kege::ui::ID> ids;     // items being dragged (usually == current selection)
        int type = 0;            // caller-defined tag so drop targets can filter accepted payloads
        void* user_data = nullptr;
    };

    class SelectionController
    {
    public:

        void updateMarqueeRectangule();
        
        // --- Selection ------------------------------------------------------
        // Call from inside a selectable widget after you've computed its click/hover state.
//        void selectable(kege::ui::ID id, bool left_released, bool right_released);
//        void clearSelection();
        const std::unordered_set<kege::ui::ID>& selection() const { return _selected; }

        // --- Drag press ------------------------------------------------------
        // Call from inside a *draggable* widget, passing whether the mouse is down
        // and hovering it this frame. This just arms the drag; update() advances it.
//        void draggable(kege::ui::ID id, bool hovered, const Rect& screen_rect, int drag_type = 0);

        // --- Per-frame update (call once, after all widgets for the frame are built) ---
        void update();

        bool isDragging() const { return _phase == kege::ui::DragPhase::Dragging; }
//        const kege::ui::DragPayload& payload() const { return _payload; }
//        kege::dvec2 dragOffset() const { return _input->getMouse()->getPosition() - _press_pos; }

        // --- Drop targets ------------------------------------------------------
        // Call during a container's layout pass (e.g. each dock zone) with its screen rect.
        void registerDropTarget(ID id, int type, void* payload);
        // Call after update(), from whoever owns that target id, to consume the drop.
        // Returns true (and fills payload) only on the frame the drop happened.
        //bool consumeDrop(ID target_id, DragPayload& out_payload);

        //void cancelDrag(); // e.g. wire to Escape

        bool isSelected(kege::ui::ID id) const;

        SelectionController(kege::GUI* gui);

    private:

        void updateSelectionList();
        void updateDragging();
        void drawMarquee();


        struct DropZone { kege::ui::ID id; kege::ui::Rect rect; int accept_type; };

        const kege::InputManager* _input;
        kege::GUI* _gui;

        kege::ui::DragPhase _phase = DragPhase::Idle;

        std::unordered_set<kege::ui::ID> _selected;

        kege::ui::ID _armed_id{};
        kege::ui::Rect _armed_rect{};
        kege::ui::Rect _marquee_rect{};
        int _armed_type = 0;
        kege::dvec2 _press_pos{};
        float _drag_threshold = 4.0f;

        kege::ui::DragPayload _payload;

        std::vector< DropZone > _zones_this_frame;   // rebuilt every frame
        kege::ui::ID _dropped_target{};            // set on the frame a drop lands
        bool _drop_pending = false;
        bool _button_down = false;
    };

}
#endif /* drag_drop_controller_hpp */
