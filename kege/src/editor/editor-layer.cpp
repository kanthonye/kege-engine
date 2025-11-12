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
        _input.processInputs( _input_context_manager->getCurrentInputs() );
        _layout->begin( &_input );
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

        addPanel( new HierarchyPanel( _project_manager, _layout.ref() ) );
        addPanel( new InspectorPanel( _project_manager, _layout.ref() ) );
        addPanel( new ViewportPanel( _project_manager, _layout.ref() ) );

        _root = new ui::DockNode( _layout.ref(), "dock" );
        _root->_panels.push_back(_panels[0]);
        _root->_panels.push_back(_panels[1]);
        _root->_panels.push_back(_panels[2]);
//        _main_panel = new kege::DockingPanel( _project_manager, &_layout );

//            _panels.push_back( new NavbarPanel( pm, l ) );
//            _panels.push_back( new NavbarPanel( pm, l ) );
//
//        _hierarchy_panel.init( &_engine, _layout );
//        _inspector_panel.init( &_engine, _layout );
//        _viewport_panel.init( &_engine, _layout );
//        _navbar_panel.init( &_engine, _layout );
//
//        Entity entity = Entity::create();
//        entity.add< Transform >({});
//        entity.add< Terrain >()->initialize
//        ({
//            .type = TerrainType::SPHERICAL,
//            .radius = 512,
//            .maximum_depth = 5,
//            .minimum_depth = 0,
//            .maximum_height = 0,
//            .minimum_height = 0,
//            .maximum_resolution = 8,
//            .graphics = _engine.graphics().get()
//        });
//        entity.add< Geometry >
//        ({
//            .mesh = entity.add< Terrain >()->getTerrainRenderer(),
//            .material = new Material({ entity.add< Terrain >()->getTerrainMaterial() })
//        });
//        _engine.scene().getScene()->insert( entity );
        return true;
    }

    void EditorLayer::shutdown()
    {
        _viewer.shutdown();
    }

    EditorLayer::EditorLayer( kege::AssetManager* am, kege::RenderGraph* rg, kege::ProjectManager* pm, kege::InputContextManager* icm )
    :   kege::AppLayer( "EditorLayer" )
    ,   _input_context_manager( icm )
    ,   _asset_manager( am )
    ,   _project_manager( pm )
    ,   _render_graph( rg )
    ,   _paused( false )
    {
    }
}
