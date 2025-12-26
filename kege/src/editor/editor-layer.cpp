//
//  editor.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#include "editor-layer.hpp"
#include "render-graph.hpp"
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

    float v[5] = {0,0.2,0.2,0,0};
    kege::UID id;
    kege::UID slider[2];
    kege::UID slider1[2];
    kege::UID slider2[2];
    kege::UID scrubber[2];
    kege::UID numeric[3];
    kege::UID tex[2];
    kege::string text;
    int selection = -1;
    bool state = false;
    int mode[5];
    char num[32];
    ui::Style style2 = {
        .background = ui::Background(0xFFFFFF20),
        .height = ui::fixed(200),
        .width = ui::percent(25),
    };
    ui::Style style = {
        .background = ui::Background(0xFFFFFF20),
        .height = ui::fixed(200),
        .width = ui::fixed(200),
    };
    ui::Style style_main = kege::ui::Style{
        .background = ui::Background(0x220022FF),
        .align_text =  ui::AlignText::Right,
        .height = ui::flexible(),
        .width = ui::extend(),
        .font_size = 20,
        .gap = {4,4},
        .align =
        {
            .flow.x = ui::AlignDirX::WTE,
            .flow.y = ui::AlignDirY::NTS,
            .origin = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
            .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
            .direction = ui::AlignDir::HORIZONTAL,
            .wrap_around = true,
        },
    };
    std::vector<std::pair< kege::UID, std::string >> fruits_list = {
        {kege::UID{},"blueberries"},
        {kege::UID{},"raspberries"},
        {kege::UID{},"strawberries"},
        {kege::UID{},"mangos"},
        {kege::UID{},"apples"},
        {kege::UID{},"grapes"},
    };

    char fps_text[32];
    double sum = 0.f;
    int count = 0;


    void EditorLayer::addPanel( Ref< ui::Panel > panel )
    {
        _panel_name_index_map[ panel->getName() ] = _panels.size();
        _panels.push_back( panel );
    }

    void EditorLayer::update()
    {
        _input.update( _input_context_manager->getCurrentInputs() );
        _layout->begin( 0.016, &_input );
        //_root->update( this );

        //_dock_mngr->update(_dock_mngr->getRoot()->child[0]);
        auto begin = Clock::now();
        _dock_mngr->update();
        auto end = Clock::now();

//        auto begin = Clock::now();
//        {
//            _gui.push({.style = &style_main});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style2});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.put({.style = &style});
//            _gui.pop();
//        }
//        auto end = Clock::now();
//
        double ms = std::chrono::duration<double, std::milli>(end - begin).count();
        sum += ms;
        count += 1;
        if (count  >= 60)
        {
            sum /= double(count);
            snprintf(fps_text, 31, "%f ms", sum);
            sum = 0.0;
            count = 0;
        }
        //std::cout << name << ": " << ms << " ms\n"; \
//        _gui.push({.style = &style_main});
////        _gui.tab( fruits_list, selection );
////        _gui.textField(tex, mode[0], text );
////        _gui.scrubber(scrubber, mode[2], v[4] );
////        _gui.slider(slider, &v[0], 0, 2);
////        _gui.slidebar(slider1, &v[1], 0, 2);
////        _gui.numSlideBar(slider2, &v[2], 0, 2);
////        _gui.numeric(numeric, mode[1], v[3] );
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.put({.style = &style});
//        _gui.pop();

        _layout->end();

        _viewer.begin();
        _viewer.collectVisibleWidgets( *_layout );
        _viewer.drawText({0.f,0.f}, 200, 20, {1,1,1,1}, true, fps_text, {0,0,1000,400});
        _viewer.end(_render_graph->getRenderExecutor().ref());
    }

    bool EditorLayer::initialize()
    {
        _project_manager->createProject("");
        _project_manager->getSceneManager()->createScene("scene");
        _project_manager->getSceneManager()->changeScene("scene");

        kege::Graphics* graphics = _render_graph->getGraphics();
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

        _layout = new ui::Layout( graphics->getWindow()->getWidth(), graphics->getWindow()->getHeight() );

        if( !_layout->loadStyles( kege::vfs( "config/style.json" ).c_str() ) )
        {
            kege::Log::error << "Failed to load ui style.json" << Log::nl;
            return false;
        }
        _layout->setFont( font );
        _layout->resize( 200 );

        _gui.initialize(_layout);

        _dock_mngr = new ui::DockManager( &_gui );
        ui::Dock* root = _dock_mngr->getRoot();
        _dock_mngr->splitNode(root, 0.28, ui::SplitDirection::VERTICAL);
        _dock_mngr->splitNode(root->child[0].ref(), 0.65, ui::SplitDirection::HORIZONTAL);
        _dock_mngr->dock(root->child[0]->child[0], ui::DockZone::TAB, new HierarchyPanel( _project_manager, &_gui, _ecs ));
        _dock_mngr->dock(root->child[0]->child[1], ui::DockZone::TAB, new InspectorPanel( _project_manager, &_gui, _ecs ));
        _dock_mngr->dock(root->child[1], ui::DockZone::TAB, new ViewportPanel( _project_manager, &_gui, _ecs ));

        //hp = new HierarchyPanel( _project_manager, &_gui, _ecs );
        //std::cout << sizeof(ui::Callback)<<" style:"<< sizeof(ui::Style) <<" widgit:"<< sizeof(ui::Widget) <<" sum:" << sizeof(ui::Style) + sizeof(ui::Widget) <<"\n";
        return true;
    }

    void EditorLayer::shutdown()
    {
        _panel_name_index_map.clear();
        _panels.clear();

        _dock_mngr.clear();
        _viewer.shutdown();
        _layout.clear();
    }

    EditorLayer::EditorLayer( kege::AssetManager* am, kege::RenderGraph* rg, kege::ProjectManager* pm, kege::InputContextManager* icm, kege::ECS* ecs )
    :   kege::AppLayer( "EditorLayer" )
    ,   _input_context_manager( icm )
    ,   _asset_manager( am )
    ,   _project_manager( pm )
    ,   _render_graph( rg )
    ,   _paused( false )
    ,   _ecs( ecs )
    {
    }
}
