//
//  ui-docking-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#ifndef ui_docking_manager_hpp
#define ui_docking_manager_hpp

#include "ui-dock-context.hpp"

namespace kege::ui{

    struct Dock : public kege::RefCounter
    {
    public:

        DockSplit* split(ui::AlignDir dir, const std::vector< int >& a, const std::vector< int >& b);

        void onWindowResize(int width, int height);
        void update();
        void merge();

        Dock(ui::DockContext* context, int width, int height);

        ~Dock();
        Dock();

    private:

        void updateDragAndDrop();
        void updateContent();
        void updateTabs();

        static void ghostDraggingOp(ui::Layout* layout, const ui::UID* id, void* data);
        static void ghostDropoffOp(ui::Layout* layout, const ui::UID* id, void* data);

        void setDockPanels(const std::vector< int >& panels);
        //void addPanelToDock(const Ref< ui::Panel >& panel);

        bool isSibling(const ui::Dock& a, const ui::Dock& b);

        DropZone determineDropZone(Dock& target, const kege::dvec2& pointer);
        ui::Dock* findDropTarget(ui::Dock& dock);
        void resolveSiblingDropOff(ui::Dock* target);
        void resolveSelfDropOff(ui::Dock* target);
        void resolveTabDropOff(ui::Dock* target);
        void splitDropOffTarget(ui::Dock* target);
        bool handleDropOff(ui::Dock& node);

    public:

        ui::DockContext* _context;

        /**
         * rect: hold the position and size of the ui dock element
         */
        Rect _rect;

        Padding _padding;

        /**
         * uid: hold the unique identifier for this dock
         */
        UID _uid;

        /**
         * tab: hold the tab list of panels in this dock
         */
        Tab _tab;

        kege::ui::Style _style;

        /**
         * parent: hold the parent dock node
         */
        Dock* _parent;

        /**
         * split: hold the split information if this dock is splitted
         */
        Ref< DockSplit > _split;

        /**
         * name: hold the name of the dock
         */
        ui::Text _name;

        friend DockManager;
    };
    
}


namespace kege::ui{

    struct DockSplit : public kege::RefCounter
    {
        DockSplit(Dock& dock, float slit_ratio, ui::AlignDir direction);
        ~DockSplit();

        void updateHorizontalDragRect();
        void updateVerticalDragRect();
        void onReshape();
        
        void onResizeGrandChild(ui::AlignDir direction, const Rect& drag_rect);
        void onResize(ui::AlignDir direction, const Rect& drag_rect);
        bool update(GUI* gui);

        /**
         * nodes: hold the two child docks of this split
         */
        Dock nodes[2];
        Dock* owner;

        /**
         * drag_rect: hold the position and size of the ui dock split element,
         * this is the area between two docks that allows resizing
         */
        ui::Rect drag_rect;

        /**
         * slit_ratio: hold the split ratio between the two docks
         */
        float slit_ratio;

        /**
         * drag_resize: hold the drag resize state
         */
        bool drag_resize;

        /**
         * direction: hold the split direction
         */
        ui::AlignDir direction;


        float gap;
    };
}

#endif /* ui_docking_manager_hpp */
