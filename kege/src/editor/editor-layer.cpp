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
//#include "bench.h"
//
//using Clock = std::chrono::high_resolution_clock;
//double bench_repeat(const char *name, size_t iterations, std::function<void()> fn)
//{
//    auto begin = Clock::now();
//    for (size_t i = 0; i < iterations; i++)
//        fn();
//    auto end = Clock::now();
//
//    double total = std::chrono::duration<double, std::milli>(end - begin).count();
//    std::cout << name << ": [ Avg Time: " << (total / iterations) << " ms "
//    << "| iterations: " << iterations << " | total time to complete: ("
//    << total << " ms) ]\n";
//
//    return total;
//}

namespace kege{

    void EditorLayer::operator()(const kege::WindowFrameBufferSizeEvent& event)
    {
        _viewer.onWindowFrameBufferResize(event.width, event.height);
    }

    void EditorLayer::operator()(const kege::WindowSizeEvent& event)
    {
        _viewer.onWindowResize(event.width, event.height);
        _layout->onWindowResize(event.width, event.height);
        _dock_manager->onWindowResize(event.width, event.height);
    }

    ui::UID uid;
    bool EditorLayer::update()
    {
        _gui.begin( 0.016 );
        _dock_manager->update();
        _gui.end();
//        _gui.push({
//            .layer = 0,
//            .rect = {200,200,500, 50},
//            .color = 0xFFFFFF18,
//            .alignment = {
//                .direction = ui::AlignDir::DOWN
//            },
//            .gap = {5,5},
//            .clip_overflow = false,
//        });
//            ui::ID id[3] = {uid[0],uid[1],uid[2]};
//            _gui.beginScrollContainer(id, 0);
//                _gui.put({.layer = 0, .rect = {0,0,100, 20}, .color = 0xFFFFFF18});
//                _gui.put({.layer = 0, .rect = {0,0,100, 20}, .color = 0xFFFFFF18});
//                _gui.put({.layer = 0, .rect = {0,0,100, 20}, .color = 0xFFFFFF18});
//                _gui.put({.layer = 0, .rect = {0,0,100, 20}, .color = 0xFFFFFF18});
//                _gui.put({.layer = 0, .rect = {0,0,100, 20}, .color = 0xFFFFFF18});
//                _gui.put({.layer = 0, .rect = {0,0,100, 20}, .color = 0xFFFFFF18});
//            _gui.endScrollContainer();
//        _gui.pop();

        _viewer.begin();
        _viewer.render( *_layout );
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

    bool EditorLayer::initialize()
    {
        // Validate all dependencies
        if (!_project_manager || !_render_graph || !_asset_manager) {
            kege::Log::error << "Null dependency in EditorLayer" << Log::nl;
            return false;
        }

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

        _dock_manager = new kege::ui::DockManager(this, window_size.width, window_size.height);
        // TODO: SettingPanel, ConsolePanel
        return true;
    }

    void EditorLayer::shutdown()
    {
        _dock_manager.clear();
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
