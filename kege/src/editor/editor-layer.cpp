//
//  editor.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

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
        _panel_name_index_map[ panel->getName() ] = _panels.size();
        _panels.push_back( panel );
    }

    bool EditorLayer::update()
    {
        _gui.begin( 0.016 );
        _dock.update();
        _gui.end();

        _viewer.begin();
        _viewer.render( *_layout );
        //_viewer.drawText({0.f,0.f}, 200, 20, 0xFFFFFFFF, true, fps_text, {0,0,1000,400});
        _viewer.end(_render_graph->getRenderExecutor().ref());
        return true;
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


        _context = new ui::DockContext( _project_manager, &_gui, _ecs );
        _dock = ui::Dock(_context.ref(), window_size.width, window_size.height);

        ui::DockSplit* split[2];

        split[0] = _dock.split( ui::AlignDir::HORIZONTAL, {}, {} );
        split[0]->nodes[0].split( ui::AlignDir::VERTICAL, {0}, {1} );
        split[1] = split[0]->nodes[1].split( ui::AlignDir::VERTICAL, {2}, {3,4,5} );

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
