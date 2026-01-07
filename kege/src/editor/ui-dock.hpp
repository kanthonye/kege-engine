//
//  ui-docking-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#ifndef ui_docking_manager_hpp
#define ui_docking_manager_hpp

#include "ui-tab.hpp"
#include "ui-panel.hpp"

namespace kege{
    class EditorLayer;
}
namespace kege::ui{

    class DockManager;

    enum DockZone {
        CENTER,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        TAB,
        FLOATING
    };

    struct DockDesc
    {
        std::vector< Ref< ui::Panel > > panels;
        std::vector< DockDesc > docks;
        int style_indices[2];
    };

    struct TabElem
    {
        ui::UID uids[3];
        ui::Text text;
    };

    struct Tab
    {
        /**
         * panels: hold the list of panels in this dock
         */
        std::vector< Ref< ui::Panel > > panels;
        std::vector< TabElem > list;
        int selection = 0;
        UID uid;
    };

    struct DockNode;

    struct DockSplit;
    struct Dock : public kege::RefCounter
    {
        Dock();

        /**
         * rect: hold the position and size of the ui dock element
         */
        Rect rect;

        Padding padding;

        /**
         * uid: hold the unique identifier for this dock
         */
        UID uid;

        /**
         * tab: hold the tab list of panels in this dock
         */
        Tab tab;

        kege::ui::Style style;
        
        /**
         * parent: hold the parent dock node
         */
        Dock* parent;

        /**
         * split: hold the split information if this dock is splitted
         */
        Ref< DockSplit > split;

        /**
         * name: hold the name of the dock
         */
        ui::Text name;
    };

    struct DockSplit : public kege::RefCounter
    {
        DockSplit(Dock& dock, float slit_ratio, ui::AlignDir direction);
        void updateHorizontalDragRect();
        void updateVerticalDragRect();
        void onResizeGrandChild(ui::AlignDir direction, const Rect& drag_rect, Dock& dock);
        void onResize(ui::AlignDir direction, const Rect& drag_rect, Dock& dock);
        bool update(GUI* gui, Dock& dock);

        /**
         * nodes: hold the two child docks of this split
         */
        Dock nodes[2];
        Dock* dock;
        
        /**
         * rect: hold the position and size of the ui dock split element,
         * this is the area between two docks that allows resizing
         */
        ui::Rect rect;

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

    struct ResizeHandler
    {
        Dock* dock;
    };

    struct GhostObject
    {
        ui::Text text;
        UID* target = nullptr;
        Rect rect;
        bool visible = false;
        bool dragging = false;
        ui::Dock* dock;
    };
    struct GhostParam
    {
        GhostObject* ghost;
    };

    struct GhostDropoff
    {
        GhostObject* ghost;
        DockManager* manager;
    };

    class DockManager : public kege::RefCounter
    {
    public:

        void split(Dock& node, ui::AlignDir dir, const std::vector< Ref< ui::Panel > >& a, const std::vector< Ref< ui::Panel > >& b);
        void merge(Dock& node);

        void setDockPanels(ui::Dock& node, const std::vector< Ref< ui::Panel > >& panels);
        void addPanelToDock(ui::Dock& node, const Ref< ui::Panel >& panel);
        void switchSpot(ui::Dock& a, ui::Dock& b);

        void updateTabs(ui::Dock& node);
        void update(ui::Dock& node);
        void update();


        ui::Dock& getRoot();

        DockManager(kege::GUI* gui, int width, int height);

    private:

        static void ghostDraggingOp(ui::Layout* layout, const ui::UID& id, void* data);
        static void ghostDropoffOp(ui::Layout* layout, const ui::UID& id, void* data);
        void handleSiblingDropSpot(ui::Dock& node, float lr, float tb);
        bool handleDropOff(ui::Dock& node);
        bool textRect(const Rect& m, const Rect& n);

        enum {DOCK, PANEL, TAB_LIST, TAB_ELEM, TAB_LABEL, CLOSE, CLOSE_HOT, DOCK_PREVIEW, TOTAL_STYLES };

        std::vector< Ref< ui::Dock > > _nodes;
        ui::Dock _root;
        kege::GUI* _gui;

        kege::ui::Style _styles[ TOTAL_STYLES ];

        GhostObject _ghost;
        ui::UID uid;
        //DockPreview preview;
    };
}
#endif /* ui_docking_manager_hpp */
