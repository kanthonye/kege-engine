//
//  editor.hpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#ifndef editor_hpp
#define editor_hpp

#include "dock/ui-dock-split.hpp"

namespace kege{

    class EditorLayer : public kege::AppLayer
    {
    public:

        void operator()(const kege::WindowFrameBufferSizeEvent& event);
        void operator()(const kege::WindowSizeEvent& event);

        kege::InputManager* getInputManager();
        kege::ProjectManager* getProjectManager();
        kege::AssetManager* getAssetManager();
        kege::RenderGraph* getRenderGraph();
        kege::ECS* getECS();
        kege::GUI* getGUI();

        std::vector< int32_t > getPanelIndice( const std::vector< std::string >& names);
        int32_t getPanelIndex( const std::string& name );
        ui::Panel* getPanel( uint32_t index );
        ui::GhostObject* getGhostObject();

        void displayPanel( uint32_t index );
        bool initialize();
        void shutdown();
        bool update();

        EditorLayer( kege::AssetManager* am, kege::RenderGraph* rg, kege::ProjectManager* pm, kege::InputManager* im, kege::ECS* ecs );
        ~EditorLayer();
        
    public:

        void addPanel( Ref< ui::Panel > panel );

    public:

        std::map< std::string, int32_t > _panel_name_index_map;
        std::vector< Ref< ui::Panel > > _panels;

        kege::ProjectManager* _project_manager;
        kege::InputManager* _input_manager;
        kege::AssetManager* _asset_manager;
        kege::RenderGraph* _render_graph;
        kege::ECS* _ecs;

        ui::Dock _dock;

        kege::Ref< ui::Layout > _layout;
        kege::GUI _gui;

        ui::Viewer _viewer;

        ui::GhostObject _ghost;

        const ImageDefn* _color_image_defn;
        ref::Sampler _sampler;
        bool _paused;
    };
}

#endif // editor_hpp
