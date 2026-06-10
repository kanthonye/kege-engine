//
//  editor.hpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#ifndef editor_hpp
#define editor_hpp

#include "shader-compiler.hpp"
#include "ui-executor.hpp"
#include "dock/ui-dock-manager.hpp"
#include "renderer.hpp"

namespace kege{

    class EditorLayer : public kege::AppLayer
    {
    public:

        //void operator()(const kege::WindowFrameBufferSizeEvent& event);
        //void operator()(const kege::WindowSizeEvent& event);

        //kege::ProjectManager* getProjectManager();
        //kege::InputManager* getInputManager();
        //kege::AssetManager* getAssetManager();
        //kege::Renderer* getRenderer();
        //kege::ECS* getECS();
        kege::UI* getUI();

        bool initialize();
        void shutdown();

        bool update();

        EditorLayer(kege::Renderer* r, kege::ProjectManager* pm, kege::GUI* ui, kege::ECS* ecs);
        ~EditorLayer();
        
    public:

        void addPanel( Ref< ui::Panel > panel );

    public:

        kege::Ref< kege::ui::DockManager > _dock_manager;
        kege::array< ui::Command > _commands;

        kege::Theme _theme;
        kege::UI _ui;
    };
}

#endif // editor_hpp
