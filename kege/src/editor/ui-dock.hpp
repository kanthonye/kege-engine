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

    enum DockZone {
        CENTER,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        TAB,
        FLOATING
    };

    enum SplitDirection {
        HORIZONTAL,
        VERTICAL,
        NONE
    };

    struct DockNode {
        //Rectangle bounds;
        DockNode* child[2];
        DockNode* parent;
        //Window* window;
        float splitRatio;
        SplitDirection splitDir;
        bool isLeaf;
    };

    struct DockDesc
    {
        std::vector< Ref< ui::Panel > > panels;
        std::vector< DockDesc > docks;
        int style_indices[2];
    };



    struct DockTab
    {
        kege::UID uids[3];
        std::string name;
    };

    struct Dock : public kege::RefCounter
    {
        Dock();
        std::vector< Ref< ui::Panel > > panels;
        std::vector< DockTab > tabs;
        int selection;

        Ref< ui::Dock > child[2];
        int count;

        kege::ui::Style style;
        SplitDirection split_dir;
        float split_ratio;

        std::string name;
        UID uid;
    };

    class DockManager : public kege::RefCounter
    {
    public:

        Ref< ui::Dock > create( const std::string& name );
        void dock(Ref< ui::Dock > node, DockZone zone, Ref< ui::Panel > panel);

        void splitNode(Dock* node, float split_ratio, SplitDirection dir);
        void mergeNode(Dock* node);

        void beginDrag(Dock* node);
        void handleDrop();

        void update(Ref< ui::Dock >& node);
        void updateTabs(Ref< ui::Dock >& node);
        void update();


        ui::Dock* getRoot();

        DockManager(kege::GUI* gui);

    private:

        enum {PANEL, TAB_LIST, TAB_ELEM, TAB_LABEL, CLOSE, CLOSE_HOT, DOCK_PREVIEW, TOTAL_STYLES };

        std::vector< Ref< ui::Dock > > _nodes;
        Ref< ui::Dock > _root;
        ui::Dock* _dragging_node;
        kege::GUI* _gui;

        kege::ui::Style _styles[ TOTAL_STYLES ];
        ui::Coord _offset;
        bool _dragging;

        kege::UID uid;
        //DockPreview preview;
    };
}
#endif /* ui_docking_manager_hpp */
