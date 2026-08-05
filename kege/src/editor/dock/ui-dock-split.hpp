//
//  ui-dock-split.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/13/26.
//

#ifndef ui_dock_split_hpp
#define ui_dock_split_hpp

#include "ui-dock.hpp"

namespace kege::ui{

    struct DockSplit : public kege::RefCounter
    {
        DockSplit(Dock& dock, float slit_ratio, ui::Dock::SplitDirection direction);
        ~DockSplit();

        void updateHorizontalDragRect();
        void updateVerticalDragRect();
        void onReshape();

        void onResizeGrandChild(ui::Dock::SplitDirection direction, const Quad& drag_rect);
        void onResize(ui::Dock::SplitDirection direction, const Quad& drag_rect);
        bool update(UI* ui);

        /**
         * nodes: hold the two child docks of this split
         */
        Dock nodes[2];
        Dock* owner;

        /**
         * drag_rect: hold the position and size of the ui dock split element,
         * this is the area between two docks that allows resizing
         */
        ui::Quad drag_quad;

        /**
         * slit_ratio: hold the split ratio between the two docks
         */
        float slit_ratio;

        /**
         * show_drag_knob: hold the drag resize state
         */
        bool show_drag_knob;

        /**
         * direction: hold the split direction
         */
        ui::Dock::SplitDirection direction;


        float gap;
    };
}

#endif /* ui_dock_split_hpp */
