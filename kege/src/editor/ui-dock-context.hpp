//
//  ui-dock-context.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/18/26.
//

#ifndef ui_dock_context_hpp
#define ui_dock_context_hpp

#include "ui-panel.hpp"
namespace kege{
    class EditorLayer;
}

namespace kege::ui{

    class DockManager;
    struct Dock;
    struct DockNode;
    struct DockSplit;

    enum DockZone {
        CENTER,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        TAB,
        FLOATING
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
        std::vector< int > panel_ids;
        std::vector< TabElem > list;
        int selection = 0;
        UID uid;
    };

    struct ResizeHandler
    {
        Dock* dock;
    };

    struct GhostObject
    {
        Rect rect;
        bool visible = false;
        bool dragging = false;
        int tab_selection;
        ui::Dock* dock;
    };

    struct GhostParam
    {
        GhostObject* ghost;
    };

    struct GhostDropoff
    {
        GhostObject* ghost;
        Dock* dock;
    };

    enum struct DropZone
    {
        NONE,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        CENTER  // For tab merging
    };

}

namespace kege::ui{

    struct DockContext : public kege::RefCounter
    {
        DockContext(kege::ProjectManager* pm, kege::GUI* gui, kege::ECS* ecs);

        // all my panels
        std::vector< Ref< ui::Panel > > panels;
        
        GhostObject ghost;
        kege::GUI* gui;

        kege::ui::Style style_dock;
        kege::ui::Style style_tab_elem;
        kege::ui::Style style_tab_label;
        kege::ui::Style style_tab;
        kege::ui::Style style_ghost;
    };
}
#endif /* ui_dock_context_hpp */
