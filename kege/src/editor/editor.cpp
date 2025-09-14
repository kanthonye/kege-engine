//
//  editor.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/7/25.
//

#include "editor.hpp"

namespace kege{

    bool Editor::initalize()
    {
        _engine.vfs().add();
        _engine.graphics().add();
        _engine.renderGraph().add();
        _engine.renderManager().add();
        _engine.input().add();
//        _engine.ecs().add();
        _engine.scene().add();

        if( !_engine.initialize() )
        {
            KEGE_LOG_ERROR << "Failed to initialize Engine." << Log::nl;
            return false;
        }

        // alert systems of the scene change

        kege::string shader_file = kege::vfs( "graphics-shaders/gui/gui-rounded-corner-sdf-text.json" );
        PipelineHandle pipeline = PipelineLoader::load( _engine.graphics().get(), shader_file.c_str() );
        if( !pipeline )
        {
            KEGE_LOG_ERROR << "Failed to load pipeline -> " << shader_file << Log::nl;
            return false;
        }

        kege::Font font = ui::FontCreator::create
        (
            _engine.graphics().get(), 8, 16,
            vfs( "assets/fonts/monaco.tga" ).c_str()
        );
        if( !font )
        {
            KEGE_LOG_ERROR << "Failed to create font." << Log::nl;
            return false;
        }

        if ( !_viewer.initialize( _engine.graphics().get(), pipeline, font ) )
        {
            return false;
        }

        Communication::add< kege::RenderPassContext*, Editor >( this );

        if( !_layout.loadStyles( kege::vfs( "root/src/editor/ui-elements/style.json" ).c_str() ) )
        {
            KEGE_LOG_ERROR << "Failed to load ui style.json" << Log::nl;
            return false;
        }
        _layout.setFont( font );
        _layout.resize( 200 );
        
        main_panel = _layout.make({
            .mouseover = false,
            .style = _layout.getStyleByName( "main")
        });
        _hierarchy_panel.init( &_engine, _layout );
        _inspector_panel.init( &_engine, _layout );
        _viewport_panel.init( &_engine, _layout );
        _navbar_panel.init( &_engine, _layout );

        SamplerHandle sampler = *_engine.renderGraph()->getPhysicalSampler( "sampler-nearest-norep" );
        _viewer.setUiImages
        ({
            ImageInfo
            {
                .image = _viewer.getDefaultTexture(),
                .sampler = sampler,
                .layout = ImageLayout::ShaderReadOnly
            },
            ImageInfo
            {
                .image = _engine.renderGraph()->getPhysicalImages( "scene_color" )->at(0),
                .sampler = sampler,
                .layout = ImageLayout::ShaderReadOnly
            }
        });
        return true;
    }

    void Editor::shutdown()
    {
        _viewer.shutdow();
        _engine.shutdown();
    }

    void Editor::loop()
    {
        bool _running = true;
        while ( _running && _engine.graphics()->windowIsOpen() )
        {
            _engine.tick();
            _engine.input()->updateCurrentInputs();

            _input.processInputs( _engine.input()->getCurrentInputs() );

            _layout.begin( &_input );
            _layout.push( main_panel );
            {
                _navbar_panel.put( _layout );

                _layout.push( _layout.make({ .visible = true, .style = _layout.getStyleByName( "content" ) }) );
                {
                    _viewport_panel.put( _layout );

                    _layout.push( _layout.make({ .visible = true, .style = _layout.getStyleByName( "side-panel" ) }) );
                    {
                        _hierarchy_panel.put( _layout );

                        _layout.push( _layout.make({ .style = _layout.getStyleByName( "inspector-panel" ) }) );
                        {
                            _inspector_panel.put( _layout );
                        }
                    }
                    _layout.pop();
                }
                _layout.pop();
            }
            _layout.pop();
            _layout.end();

            Communication::broadcast< const MappedInputs& >( _engine.input()->getMappedInputs() );

            // 4. Step engine/game systems
            if ( !_paused )
            {
                _engine.scene().update( _engine.dms() );
            }
            _engine.scene()._entity_systems->render(0);

            if ( 0 <= _engine.graphics()->beginFrame() )
            {
                _engine.renderManager()->execute( 0.f );
                if ( !_engine.graphics()->endFrame() )
                {
                    KEGE_LOG_ERROR << "Failed to end Frame" <<Log::nl;
                    _running = false;
                }
            }
            else
            {
                KEGE_LOG_ERROR << "Failed to begin Frame" <<Log::nl;
                _running = false;
            }
            
            _engine.graphics()->getWindow()->pollEvents();
        }
    }

    bool Editor::run()
    {
        if ( !initalize() )
        {
            KEGE_LOG_ERROR << "Failed to initialize Editor." << Log::nl;
            shutdown();
            return 0;
        }
        loop();
        shutdown();
        return 0;
    }

    void Editor::buildEditorPanels()
    {
    }

    void Editor::operator()( kege::RenderPassContext* context )
    {
        if( context->name() != "final-pass" )
        {
            return;
        }

        kege::CommandEncoder* encoder = context->getCommandEncoder();

        _viewer.begin( encoder );
        _viewer.draw( _layout, 1, _layout[1]->rect );
        _viewer.end();
    }

    Editor::Editor()
    :   _paused( false )
    {
    }
}
