//
//  editor.hpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#ifndef editor_hpp
#define editor_hpp

#include "../core/app/app-layer.hpp"

#include "hierarchy-panel.hpp"
#include "inspector-panel.hpp"
#include "viewport-panel.hpp"
#include "navbar-panel.hpp"
#include "ui-dock.hpp"
#include "gui.hpp"

namespace kege{

    class EditorLayer : public kege::AppLayer
    {
    public:

        bool initialize();
        void shutdown();
        void update();

        EditorLayer( kege::AssetManager* am, kege::RenderGraph* rg, kege::ProjectManager* pm, kege::InputContextManager* icm, kege::ECS* ecs );

    public:

        void addPanel( Ref< ui::Panel > panel );

    public:

        std::map< std::string, size_t > _panel_name_index_map;
        std::vector< Ref< ui::Panel > > _panels;

        kege::InputContextManager* _input_context_manager;
        kege::ProjectManager* _project_manager;
        kege::AssetManager* _asset_manager;
        kege::RenderGraph* _render_graph;
        kege::ECS* _ecs;

        kege::GUI _gui;

        Ref< ui::Panel > hp;

        Ref< ui::DockManager > _dock_mngr;

        Ref< ui::Layout > _layout;
        ui::Viewer _viewer;
        ui::Input _input;

        bool _paused;
    };
}

#endif // editor_hpp
