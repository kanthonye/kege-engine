//
//  editor.hpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#ifndef editor_hpp
#define editor_hpp

#include "../core/app/app-layer.hpp"

#include "panels/hierarchy-panel.hpp"
#include "panels/inspector-panel.hpp"
#include "panels/viewport-panel.hpp"
#include "panels/ui-file-browser.hpp"
#include "ui-dock.hpp"
#include "gui.hpp"

namespace kege{

    class EditorLayer : public kege::AppLayer
    {
    public:

        void operator()(const kege::WindowFrameBufferSizeEvent& event);
        void operator()(const kege::WindowSizeEvent& event);

        bool initialize();
        void shutdown();
        bool update();

        EditorLayer( kege::AssetManager* am, kege::RenderGraph* rg, kege::ProjectManager* pm, kege::InputManager* im, kege::ECS* ecs );
        ~EditorLayer();
        
    public:

        void addPanel( Ref< ui::Panel > panel );

    public:

        std::map< std::string, size_t > _panel_name_index_map;
        std::vector< Ref< ui::Panel > > _panels;

        kege::InputManager* _input_manager;
        kege::ProjectManager* _project_manager;
        kege::AssetManager* _asset_manager;
        kege::RenderGraph* _render_graph;
        kege::ECS* _ecs;


        Ref< ui::DockContext > _context;
        ui::Dock _dock;

        kege::GUI _gui;

        Ref< ui::Layout > _layout;
        ui::Viewer _viewer;

        const ImageDefn* _color_image_defn;
        ref::Sampler _sampler;
        bool _paused;
    };
}

#endif // editor_hpp
