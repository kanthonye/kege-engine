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
#include "panels/navbar/ui-nav-bar.hpp"
#include "renderer.hpp"

namespace kege{

    class EditorLayer : public kege::AppLayer
    {
    public:

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
        kege::Ref< kege::ui::NavBar > _navbar;
        kege::array< ui::Command > _commands;
        kege::GUI* _gui;

        kege::Extent2D _extent;
        kege::Theme _theme;
        kege::UI _ui;
    };
}

#endif // editor_hpp
