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
#include "panels/file-browser/ui-file-browser.hpp"

#include "panels/console/ui-console.hpp"

#include "editor-layer.hpp"
#include "../core/graphics/render/graph/render-graph.hpp"

namespace kege{

    void testrun(GUI* _gui)
    {
        static kege::ui::UID uid;
        _gui->push
        ({
            .wid = _gui->newElem({
                .alignment = {
                    .gap = {5,5},
                    .origin = {ui::AlignX::RIGHT, ui::AlignY::TOP},
                    .direction = {ui::AlignDir::DOWN, ui::AlignDir::LEFT},
                }
            }),
            .quad =
            {
                300.f, 50.f, 800, 800,
                0x00FFFF1F,
            },
        });
        for(int i=0; i<10; i++)
        {
            _gui->put
            ({
                .user_id = uid[i],
                .quad =
                {
                    0.f, 0.f, 200, 200,
                    (_gui->mouseover(uid[i]))? 0xF000F0F0: 0xF000F030,
                },
            });
        }
        _gui->pop();
    }
    bool EditorLayer::update()
    {
        kege::ui::StateManager::update();

        _gui->begin( 0.016 );
        //testrun(_gui);
//        _gui->beginRoot();
//        _navbar->update();
        _dock_manager->update();
//        _gui->endRoot();
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
        kege::ui::Quad nav_rect = {
            0,0,
            static_cast<float>(_extent.width),
            30 - gap
        };
        kege::ui::Quad dock_rect =
        {
            0.f,
            static_cast<float>(nav_rect.height + gap),
            static_cast<float>(_extent.width),
            static_cast<float>(_extent.height - nav_rect.height - gap),
            0xFFFFFF10
        };
        _navbar = new kege::ui::NavBar( nav_rect, gui, &_ui, pm, ecs );
        _dock_manager = new kege::ui::DockManager( dock_rect, gui, &_ui, pm, ecs );
    }

    EditorLayer::~EditorLayer()
    {
    }

}
