//
//  editor.hpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#ifndef editor_hpp
#define editor_hpp

#include "dock/ui-dock-manager.hpp"

namespace kege{

    class EditorLayer : public kege::AppLayer
    {
    public:

        void operator()(const kege::WindowFrameBufferSizeEvent& event);
        void operator()(const kege::WindowSizeEvent& event);

        kege::ProjectManager* getProjectManager();
        kege::InputManager* getInputManager();
        kege::AssetManager* getAssetManager();
        kege::RenderGraph* getRenderGraph();
        kege::ECS* getECS();
        kege::GUI* getGUI();

        bool initialize();
        void shutdown();
        bool update();

        EditorLayer( kege::AssetManager* am, kege::RenderGraph* rg, kege::ProjectManager* pm, kege::InputManager* im, kege::ECS* ecs );
        ~EditorLayer();
        
    public:

        void addPanel( Ref< ui::Panel > panel );

    public:

        kege::Ref< kege::ui::DockManager > _dock_manager;
        kege::ProjectManager* _project_manager;
        kege::InputManager* _input_manager;
        kege::AssetManager* _asset_manager;
        kege::RenderGraph* _render_graph;
        kege::ECS* _ecs;

        kege::Ref< ui::Layout > _layout;
        kege::GUI _gui;

        ui::Viewer _viewer;

        const ImageDefn* _color_image_defn;
        ref::Sampler _sampler;
        bool _paused;
    };
}

#endif // editor_hpp
