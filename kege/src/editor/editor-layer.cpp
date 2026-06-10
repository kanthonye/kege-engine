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
#include "panels/ui-menu-bar.hpp"

#include "editor-layer.hpp"
#include "../core/graphics/render/graph/render-graph.hpp"

namespace kege{

    bool EditorLayer::update()
    {
        kege::ui::StateManager::update();
        _dock_manager->update();
        return true;
    }

    bool EditorLayer::initialize()
    {
        // TODO: SettingPanel, ConsolePanel
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
    {
        _ui.init(gui, &_theme);
        kege::Extent2D extent = r->getWindow()->getSize();
        _dock_manager = new kege::ui::DockManager(extent, gui, &_ui, pm, ecs );
    }

    EditorLayer::~EditorLayer()
    {
    }

}
