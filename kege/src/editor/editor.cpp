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
        _engine.input().add();
        _engine.ecs().add();
        _engine.esm().add();
        _engine.scene().add();

        if( !_engine.initialize() )
        {
            KEGE_LOG_ERROR << "Failed to initialize Engine." << Log::nl;
            return false;
        }

        // alert systems of the scene change
        _engine.esm()->onSceneChange();

        kege::Font font = ui::FontCreator::create
        (
            _engine.graphics().get(), 8, 16,
            _engine.vfs()->fetch( "assets/fonts/monaco.tga" ).c_str()
        );
        if( !font )
        {
            KEGE_LOG_ERROR << "Failed to create font." << Log::nl;
            return false;
        }

        kege::string shader_file = _engine.vfs()->fetch( "graphics-shaders/gui/gui-rounded-corner-sdf-text.json" );
        PipelineHandle pipeline = PipelineLoader::load( _engine.graphics().get(), shader_file.c_str() );
        if( !pipeline )
        {
            KEGE_LOG_ERROR << "Failed to load pipeline -> " << shader_file << Log::nl;
            return false;
        }

        if ( !_viewer.initialize( _engine.graphics().get(), pipeline, font ) )
        {
            return false;
        }

        Communication::add< kege::RenderPassContext*, Editor >( this );

        _layout.resize( 200 );

        main_panel = _layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0x000000FF),
                .width = ui::fixed( _engine.graphics()->windowWidth() ),
                .height = ui::fixed( _engine.graphics()->windowHeight() ),
                .padding = {8,8,8,8},
                .gap = {2,2},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::VERTICALLY,
                    .wrap_around = false
                },
            }
        });
        _hierarchy_panel.init( &_engine, _layout );
        _inspector_panel.init( &_engine, _layout );
        _viewport_panel.init( &_engine, _layout );
        _navbar_panel.init( &_engine, _layout );
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

            // 2. Build UI layout for editor panels
            _layout.begin( &_input );
            buildEditorPanels();
            _layout.end();

            // 4. Step engine/game systems
            if ( !_paused )
            {
                _engine.esm()->update( _engine.dms() );
            }

            if ( 0 <= _engine.graphics()->beginFrame() )
            {
                _engine.renderGraph()->execute();
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
        _layout.push( main_panel );
        {
            _navbar_panel.put( _layout );

            _layout.push(_layout.make
             ({
                 .style =
                 {
                     .background = ui::bgColor(0x1B1A1700),
                     .width = ui::extend(),
                     .height = ui::extend(),
                     .padding = {},
                     .gap = {2,2},
                     .align =
                     {
                         .origin = ui::ALIGN_TOP_CENTER,
                         .direction = ui::HORIZONTALLY,
                         .wrap_around = false
                     },
                 }
             }));
            {
                _viewport_panel.put( _layout );

                _layout.push(_layout.make
                 ({
                     .style =
                     {
                         .background = ui::bgColor(0x1B1A1700),
                         .width = ui::fixed( 350 ),
                         .height = ui::extend(),
                         .padding = {},
                         .gap = {2,2},
                         .align =
                         {
                             .origin = ui::ALIGN_TOP_CENTER,
                             .direction = ui::VERTICALLY,
                             .wrap_around = false
                         },
                     }
                 }));
                {
                    _hierarchy_panel.put( _layout );
                    _inspector_panel.put( _layout );
                }
                _layout.pop();
            }
            _layout.pop();
        }
        _layout.pop();
    }

    void Editor::operator()( kege::RenderPassContext* context )
    {
        if( context->name() != "final-pass" )
        {
            return;
        }

         kege::CommandEncoder* encoder = context->getCommandEncoder();
         _viewer.view( encoder, _layout );
    }

    Editor::Editor()
    :   _paused( false )
    {
    }
}
