//
//  editor.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#include "panels/asset-manager/ui-asset-manager.hpp"
#include "panels/hierarchy/hierarchy-panel.hpp"
#include "panels/inspector/inspector-panel.hpp"
#include "panels/viewport/viewport-panel.hpp"
#include "panels/ui-file-browser.hpp"

#include "panels/ui-console.hpp"

#include "editor-layer.hpp"
#include "../core/graphics/render/graph/render-graph.hpp"

namespace kege{

    bool EditorLayer::update()
    {
        kege::ui::StateManager::update();

        _gui->begin( 0.016 );
        _gui->pushRoot
        ({
            .rect = {
                0.f, 0.f,
                static_cast<float>(_extent.width),
                static_cast<float>(_extent.height)
            },
            .padding = {10,10,10,10},
            .color = 0xFFFFFF0F,
        });
        _navbar->update();
        _dock_manager->update();
        _gui->popRoot();
        _gui->end();
        return true;
    }

    bool EditorLayer::initialize()
    {
        //TODO: SettingPanel, ConsolePanel
        //kege::ui::init();
        return true;
    }

    void EditorLayer::shutdown()
    {
        _dock_manager.clear();
    }

    EditorLayer::EditorLayer
    (
        kege::Renderer* r,
        kege::ProjectManager* pm,
        kege::GUI* gui,
        kege::ECS* ecs
    )
    :   kege::AppLayer( "EditorLayer" )
    ,   _gui( gui )
    {
        _ui.init(gui, &_theme);
        _extent = r->getWindow()->getSize();

        float gap = 4;
        kege::ui::Rect nav_rect = {
            0,0,
            static_cast<float>(_extent.width),
            30 - gap
        };
        kege::ui::Rect dock_rect =
        {
            0.f,
            static_cast<float>(nav_rect.height + gap),
            static_cast<float>(_extent.width),
            static_cast<float>(_extent.height - nav_rect.height - gap)
        };
        _navbar = new kege::ui::NavBar( nav_rect, gui, &_ui, pm, ecs );
        _dock_manager = new kege::ui::DockManager( dock_rect, gui, &_ui, pm, ecs );
    }

    EditorLayer::~EditorLayer()
    {
    }

}
