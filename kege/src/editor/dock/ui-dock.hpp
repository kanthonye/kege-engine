//
//  ui-docking-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#ifndef ui_docking_manager_hpp
#define ui_docking_manager_hpp

#include "../panels/ui-panel.hpp"

namespace kege::ui{

    struct Dock : public kege::RefCounter
    {
    public:

        kege::ui::Dock* getDock(kege::ui::Dock& dock, const kege::dvec2& pointer);
        kege::ui::Dock* getDock(const kege::dvec2& pointer);
        
        void operator()(const kege::ui::AssetMetadataDropOff& event);

        enum SplitDirection {HORIZONTAL, VERTICAL};

        DockSplit* split(float slit_ratio, ui::Dock::SplitDirection dir, const std::vector< std::string >& a, const std::vector< std::string >& b);
        DockSplit* split(float slit_ratio, ui::Dock::SplitDirection dir, const std::vector< int >& a = {}, const std::vector< int >& b = {});

        void onWindowResize(int width, int height);
        void update();
        void merge();

        Dock(kege::ui::DockManager* manager, int width, int height);

        ~Dock();
        Dock();

    private:

        void updateDragAndDrop();
        void displayContent();
        void displayTabs();

        static void ghostDraggingOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data);
        static void ghostDropoffOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data);

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

        kege::ui::DockManager* _manager;

        ui::UID _uid_ghost;
        ui::UID _uid_dock;
        ui::UID _uid_tab;

        /**
         * rect: hold the position and size of the ui dock element
         */
        Rect _rect;

        Padding _padding;

        /**
         * uid: hold the unique identifier for this dock
         */
        WidgetId _uid;

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

        kege::UI* _ui;

        friend DockSplit;
    };
    
}
#endif /* ui_docking_manager_hpp */
