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

#include "ui-console.hpp"
#include "ui-menu-bar.hpp"

#include "editor-layer.hpp"
#include "render-graph.hpp"
#include "shader-library.hpp"
#include "bench.h"

using Clock = std::chrono::high_resolution_clock;
double bench_repeat(const char *name, size_t iterations, std::function<void()> fn)
{
    auto begin = Clock::now();
    for (size_t i = 0; i < iterations; i++)
        fn();
    auto end = Clock::now();

    double total = std::chrono::duration<double, std::milli>(end - begin).count();
    std::cout << name << ": [ Avg Time: " << (total / iterations) << " ms "
    << "| iterations: " << iterations << " | total time to complete: ("
    << total << " ms) ]\n";

    return total;
}

namespace kege{

    void EditorLayer::operator()(const kege::WindowFrameBufferSizeEvent& event)
    {
        _viewer.onWindowFrameBufferResize(event.width, event.height);
    }

    void EditorLayer::operator()(const kege::WindowSizeEvent& event)
    {
        _viewer.onWindowResize(event.width, event.height);
        _layout->onWindowResize(event.width, event.height);
        _dock.onWindowResize(event.width, event.height);
    }

    void EditorLayer::addPanel( Ref< ui::Panel > panel )
    {
        _panel_name_index_map[ panel->getName() ] = (int32_t)_panels.size();
        _panels.push_back( panel );
    }

    void EditorLayer::displayPanel( uint32_t index )
    {
        if ( index < _panels.size() )
        {
            _panels[ index ]->update();
        }
    }

    bool EditorLayer::update()
    {
        _gui.begin( 0.016 );
        _dock.update();

//        _gui.push({
//            .rect = {0,0,420,420},
//            .color = 0xFFFFFF0F,
//            .gap = {2,2},
//            .padding = {20,20,20,20},
//            .alignment = ui::Alignment
//            {
//                .wrap = ui::AlignWrap{.direction = ui::AlignDir::UP, .enable = true},
//                .origin = {ui::AlignX::CENTER,ui::AlignY::CENTER},
//                .direction = ui::AlignDir::CENTER,
//                .items = ui::AlignItem::CENTER,
//            }
//        });
//        _gui.put({.rect = {0,0,100,100}, .color = 0x0FFFFF0F});
//        _gui.put({.rect = {0,0,100,120}, .color = 0x0FFFFF0F});
//        _gui.put({.rect = {0,0,100,100}, .color = 0x0FFFFF0F});
//        _gui.put({.rect = {0,0,120,100}, .color = 0x0FFFFF0F});
//        _gui.put({.rect = {0,0,100,100}, .color = 0x0FFFFF0F});
//        _gui.put({.rect = {0,0,120,120}, .color = 0x0FFFFF0F});
//        _gui.pop();
        _gui.end();

        _viewer.begin();
        _viewer.render( *_layout );
        //_viewer.drawText({0.f,0.f}, 200, 20, 0xFFFFFFFF, true, fps_text, {0,0,1000,400});
        _viewer.end(_render_graph->getRenderExecutor().ref());
        return true;
    }

    kege::InputManager* EditorLayer::getInputManager()
    {
        return _input_manager;
    }

    kege::ProjectManager* EditorLayer::getProjectManager()
    {
        return _project_manager;
    }

    kege::AssetManager* EditorLayer::getAssetManager()
    {
        return _asset_manager;
    }

    kege::RenderGraph* EditorLayer::getRenderGraph()
    {
        return _render_graph;
    }

    kege::ECS* EditorLayer::getECS()
    {
        return _ecs;
    }

    kege::GUI* EditorLayer::getGUI()
    {
        return &_gui;
    }

    ui::GhostObject* EditorLayer::getGhostObject()
    {
        return &_ghost;
    }


    std::vector< int32_t > EditorLayer::getPanelIndice( const std::vector< std::string >& names)
    {
        std::vector< int32_t > indices;
        for (const std::string& name : names)
        {
            auto itr = _panel_name_index_map.find( name );
            if( itr == _panel_name_index_map.end() )
            {
                indices.push_back( getPanelIndex( name ) );
            }
        }
        return indices;
    }

    int32_t EditorLayer::getPanelIndex( const std::string& name )
    {
        auto itr = _panel_name_index_map.find( name );
        if( itr != _panel_name_index_map.end() )
        {
            return itr->second;
        }
        return uint32_t(-1);
    }
    
    ui::Panel* EditorLayer::getPanel( uint32_t index )
    {
        if ( index < _panels.size() )
        {
            return _panels[ index ].ref();
        }
        return nullptr;
    }

    bool EditorLayer::initialize()
    {
        kege::Graphics* graphics = _render_graph->getGraphics();

        _project_manager->createProject("");
        _project_manager->getSceneManager()->createScene("scene");
        _project_manager->getSceneManager()->changeScene("scene");

        ref::Font font = kege::FontCreator::create( graphics, 8, 16, vfs( "assets/fonts/monaco.tga" ).c_str() );
        if( !font )
        {
            kege::Log::error << "Failed to create font." << Log::nl;
            return false;
        }

        if ( !_viewer.initialize( graphics, _asset_manager, font ) )
        {
            return false;
        }

        _color_image_defn = _render_graph->getImageDefn( "color" );
        _sampler = *_asset_manager->fetch< ref::Sampler >( "default" );
        for (int frame=0; frame<1/*defn->physical_handle.size()*/; ++frame)
        {
            kege::ImageBindInfo info;
            info.image = _color_image_defn->physical_handle[frame];
            info.layout = ImageLayout::ShaderRead;
            info.sampler = _sampler;
            _viewer.setViewportImage(info, frame);
        }

        Extent2D window_size = graphics->getWindow()->getSize();
        _layout = new ui::Layout( _input_manager, window_size.width, window_size.height, 5000 );
        _layout->createLayers(5);

        _layout->setFont( font );
        _gui.initialize(_layout);

        addPanel(new ui::HierarchyPanel( this ));
        addPanel(new kege::InspectorPanel( this ));
        addPanel(new kege::ViewportPanel( this ));
        addPanel(new ui::FileBrowser( this, UI_BASE_ID(), "/Users/kae/Developer/vscode/kege-engine/kege/assets" ));
        addPanel(new ui::AssetManagerUI( this, UI_BASE_ID() ));
        addPanel(new ui::Console( this ));
        addPanel(new ui::MenuBar( this ));

        _dock = ui::Dock(this, window_size.width, window_size.height);

        ui::DockSplit* split[2];

        split[0] = _dock.split( 0.25, ui::Dock::SplitDirection::HORIZONTAL );
        split[0]->nodes[0].split( 0.50, ui::Dock::SplitDirection::VERTICAL, {"Hierarchy"}, {"Properties"} );
        split[1] = split[0]->nodes[1].split( 0.70, ui::Dock::SplitDirection::VERTICAL, {"Viewport"}, {"AssetManager", "Console", "FileBrowser"} );

        split[0]->slit_ratio = 0.25;
        split[0]->onReshape();

        split[1]->slit_ratio = 0.70;
        split[1]->onReshape();


        // TODO: SettingPanel, ConsolePanel
        return true;
    }

    void EditorLayer::shutdown()
    {
        _panel_name_index_map.clear();
        _panels.clear();

        //_dock_mngr.clear();
        _viewer.shutdown();
        _layout.clear();
    }

    EditorLayer::EditorLayer( kege::AssetManager* am, kege::RenderGraph* rg, kege::ProjectManager* pm, kege::InputManager* im, kege::ECS* ecs )
    :   kege::AppLayer( "EditorLayer" )
    ,   _input_manager( im )
    ,   _asset_manager( am )
    ,   _project_manager( pm )
    ,   _render_graph( rg )
    ,   _paused( false )
    ,   _ecs( ecs )
    {
        Communication::add<const kege::WindowFrameBufferSizeEvent&, EditorLayer>(this);
        Communication::add<const kege::WindowSizeEvent&, EditorLayer>(this);
    }

    EditorLayer::~EditorLayer()
    {
        Communication::remove<const kege::WindowFrameBufferSizeEvent&, EditorLayer>(this);
        Communication::remove<const kege::WindowSizeEvent&, EditorLayer>(this);
    }

}
