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
#include "docking-panel.hpp"

namespace kege{

    class EditorLayer : public kege::AppLayer
    {
    public:

        bool initialize();
        void shutdown();
        void update();

        EditorLayer( kege::Graphics* graphics, kege::ProjectManager* pm );

    public:

        kege::ProjectManager* _project_manager;
        kege::Graphics* _graphics;

        ref::DockingPanel _main_panel;

        ui::Layout _layout;
        ui::Viewer _viewer;
        ui::Input _input;

        bool _paused;

        ui::EID main_panel;
    };
}

#endif // editor_hpp
