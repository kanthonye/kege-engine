//
//  ui-dock-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/2/26.
//

#ifndef ui_dock_manager_hpp
#define ui_dock_manager_hpp

#include "ui-dock.hpp"
#include "ui-dock-split.hpp"
#include "../panels/selection-controller.hpp"

namespace kege::ui{

    class DockManager : public kege::RefCounter{
    public:

        DockSplit* split
        (
            float slit_ratio,
            ui::Dock::SplitDirection dir,
            const std::vector< std::string >& a,
            const std::vector< std::string >& b
        );

        DockSplit* split
        (
            float slit_ratio,
            ui::Dock::SplitDirection dir,
            const std::vector< int >& a = {},
            const std::vector< int >& b = {}
        );

        void operator()(const kege::ui::AssetMetadataDropOff& event);

        void operator()(const kege::WindowFrameBufferSizeEvent& event);
        void operator()(const kege::WindowSizeEvent& event);

        kege::ui::SelectionController* getSelectionController();
        kege::ProjectManager* getProjectManager();
        kege::GraphicsDevice* getGraphicsDevice();
        kege::AssetManager* getAssetManager();
        kege::ECS* getECS();
        kege::UI* getUI();

        std::vector< int32_t > getPanelIndice( const std::vector< std::string >& names);
        kege::ui::Dock* getDock(const kege::dvec2& pointer);
        int32_t getPanelIndex( const std::string& name );
        ui::Panel* getPanel( uint32_t index );
        ui::GhostObject* getGhostObject();

        void drawDragGhost(const kege::ui::DragPayload& payload, const kege::dvec2& position);

        void addPanel( Ref< ui::Panel > panel );
        void onWindowResize(int width, int height);
        void displayPanel( uint32_t index );
        
        void update();

        DockManager
        (
            const kege::ui::Quad& quad,
            kege::GUI* gui,
            kege::UI* ui,
            kege::ProjectManager* pm,
            kege::ECS* ecs
        );
        ~DockManager();

    private:

        std::map< std::string, int32_t > _panel_name_index_map;
        std::vector< Ref< ui::Panel > > _panels;
        kege::ProjectManager* _project_manager;

        kege::ui::SelectionController _dragdrop;
        kege::ui::GhostObject _ghost;

        kege::ui::Dock _root;

        kege::GUI* _gui;
        kege::ECS* _ecs;
        kege::UI* _ui;
        
        kege::ui::Quad _quad;
        kege::ui::UID _uid;
    };
}
#endif /* ui_dock_manager_hpp */
