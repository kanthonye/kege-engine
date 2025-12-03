//
//  editor.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#include "editor-layer.hpp"
#include "render-graph.hpp"

namespace kege{

    void EditorLayer::addPanel( Ref< ui::Panel > panel )
    {
        _panel_name_index_map[ panel->getName() ] = _panels.size();
        _panels.push_back( panel );
    }

    void EditorLayer::update()
    {
        _input.update( _input_context_manager->getCurrentInputs() );
        _layout->begin( 0.016, &_input );
        _root->update( this );
        _layout->end();

        _viewer.collectVisibleWidgets( _render_graph->getRenderExecutor().ref(), *_layout );
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

        Extent2D window_fbo_size = graphics->getWindow()->getFramebufferSize();
        _layout = new ui::Layout( window_fbo_size.width, window_fbo_size.height );

        if( !_layout->loadStyles( kege::vfs( "config/style.json" ).c_str() ) )
        {
            kege::Log::error << "Failed to load ui style.json" << Log::nl;
            return false;
        }
        _layout->setFont( font );
        _layout->resize( 200 );

        addPanel( new HierarchyPanel( _project_manager, _layout.ref(), _ecs ) );
        addPanel( new InspectorPanel( _project_manager, _layout.ref(), _ecs ) );
        addPanel( new ViewportPanel( _project_manager, _layout.ref(), _ecs ) );

        _root = new ui::DockNode( _layout.ref(), "main" );

        _root->split();
        _root->_children[0]->_container->style = _layout->getStyleByName( "dock" );
        
        _root->_children[0]->split();
        _root->_children[0]->_children[0]->_panels.push_back(_panels[0]);
        _root->_children[0]->_children[1]->_panels.push_back(_panels[1]);
        _root->_children[1]->_panels.push_back(_panels[2]);


        std::cout << sizeof(ui::Callback)<<" style:"<< sizeof(ui::Style) <<" widgit:"<< sizeof(ui::Widget) <<" sum:" << sizeof(ui::Style) + sizeof(ui::Widget) <<"\n";
        return true;
    }

    void EditorLayer::shutdown()
    {
        _viewer.shutdown();
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
