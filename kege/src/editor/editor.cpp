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
                .background = ui::bgColor(0x1B1A17FF),
                .width = ui::fixed( _engine.graphics()->windowWidth() ),
                .height = ui::fixed( _engine.graphics()->windowHeight() ),
                .padding = {},
                .gap = {},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::HORIZONTALLY,
                    .wrap_around = false
                },
            }
        });
        navbar_panel = _layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0xC1BAA633),
                .width = ui::extend(),
                .height = ui::fixed( 16 ),
                .padding = {},
                .gap = {},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::HORIZONTALLY,
                    .wrap_around = false
                },
            }
        });
        viewport_panel = _layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0xC1BAA633),
                .width = ui::extend(),
                .height = ui::extend(),
                .padding = {},
                .gap = {},
            }
        });
        hierarchy_panel = _layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0xC1BAA633),
                .width = ui::extend(),
                .height = ui::extend(),
                .padding = {},
                .gap = {},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::VERTICALLY,
                    .wrap_around = false
                },
            }
        });
        inspector_panel = _layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0xC1BAA633),
                .width = ui::extend(),
                .height = ui::extend(),
                .padding = {},
                .gap = {},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::VERTICALLY,
                    .wrap_around = false
                },
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

            // 2. Build UI layout for editor panels
            buildEditorPanels();

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
        _layout.begin( &_input );
        _layout.push( main_panel );
        {
            _layout.put( navbar_panel );

            _layout.push(_layout.make
             ({
                 .style =
                 {
                     .background = ui::bgColor(0x1B1A17FF),
                     .width = ui::extend(),
                     .height = ui::extend(),
                     .padding = {},
                     .gap = {},
                     .align =
                     {
                         .origin = ui::ALIGN_TOP_CENTER,
                         .direction = ui::VERTICALLY,
                         .wrap_around = false
                     },
                 }
             }));
            {
                _layout.put( viewport_panel );

                _layout.push(_layout.make
                 ({
                     .style =
                     {
                         .background = ui::bgColor(0x1B1A17FF),
                         .width = ui::fixed( 250 ),
                         .height = ui::extend(),
                         .padding = {},
                         .gap = { 2,2 },
                         .align =
                         {
                             .origin = ui::ALIGN_TOP_CENTER,
                             .direction = ui::VERTICALLY,
                             .wrap_around = false
                         },
                     }
                 }));
                {
                    _layout.put( hierarchy_panel );
                    _layout.put( inspector_panel );
                }
                _layout.pop();
            }
            _layout.pop();
        }
        _layout.pop();
        _layout.end();
    }

    void Editor::operator()( kege::RenderPassContext* context )
    {
        if( context->name() != "scene-output" )
        {
            return;
        }

         kege::CommandEncoder* encoder = context->getCommandEncoder();
         _viewer.view( encoder, _layout );
    }

    void Editor::drawHierarchyPanel()
    {}
    
    void Editor::drawInspectorPanel()
    {}

    void Editor::drawAssetBrowser()
    {}

    void Editor::drawViewportPanel()
    {}

    Editor::Editor()
    :   _paused( false )
    {
//        _file_browser.init( _layout );

//        ui::Background bgcolor = ui::bgColor(0x141223FF);
//        ui::Background global_bgcolor = ui::bgColor({1,1,1,0.1});
//        ui::Padding global_padding = {5,5,5,5};
//        Extent2D global_gap = {5,5};
//
//        back_button = _layout.make({});

//
//
//        _layout.styles( Properties ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::extend(),
//            .height = ui::extend(),
//            .padding = global_padding,
//            .gap = {5,5},
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_CENTER,
//                .direction = ui::HORIZONTALLY,
//                .wrap_around = false
//            },
//        };
//        _layout.styles( NavBar ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::fixed( 50 ),
//            .height = ui::extend(),
//            .padding = global_padding,
//            .border_radius = 5,
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_CENTER,
//                .direction = ui::VERTICALLY
//            },
//            .gap = global_gap,
//        };
//        _layout.styles( PropertiesTabButn ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::fixed( 50 ),
//            .height = ui::fixed( 50 ),
//            .padding = {0,0,0,0},
//            .gap = global_gap
//        };
//        _layout.styles( PropertiesPanel ) =
//        {
//            .background = global_bgcolor,
//            .width = ui::fixed( 300 ),
//            .height = ui::extend(),
//            .padding = global_padding,
//            .gap = global_gap,
//            .border_radius = 5,
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_CENTER,
//                .direction = ui::VERTICALLY,
//                .wrap_around = false
//            }
//        };
//
//        _layout.styles( ContentArea ) = ui::Style
//        {
//            .background = {},
//            .width = ui::extend(),
//            .height = ui::extend(),
//            .padding = {},
//            .gap = {2,0},
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_CENTER,
//                .direction = ui::VERTICALLY,
//                .wrap_around = false
//            },
//        };
//        _layout.styles( ViewPanel ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::extend(),
//            .height = ui::extend(),
//            .padding = global_padding,
//            .gap = global_gap,
//            .border_radius = 5,
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_RIGHT,
//                .direction = ui::HORIZONTALLY
//            }
//        };
//        _layout.styles( FileBrowserPanel ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::extend(),
//            .height = ui::fixed( 300 ),
//            .padding = global_padding,
//            .gap = global_gap,
//            .border_radius = 5,
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_CENTER,
//                .direction = ui::VERTICALLY,
//            }
//        };
//        _layout.styles( FileBrowserPanelBanner ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::extend(),
//            .height = ui::fixed( 30 ),
//            .padding = global_padding,
//            .gap = global_gap,
//            .border_radius = 5,
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_CENTER,
//                .direction = ui::HORIZONTALLY,
//            }
//        };
//
//        _layout.styles( BackButton ) = ui::Style
//        {
//            .background = ui::bgColor({1,1,1,0.1}),
//            .width = ui::fixed( 20 ),
//            .height = ui::fixed( 20 ),
//            .click_trigger = ui::ClickTrigger::OnRelease
//        };
//
//        _layout.styles( FileBrowserPath ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::extend(),
//            .height = ui::fixed( 20 ),
//            .padding = {10, 0, 0, 0},
//            .alignment =
//            {
//                .origin = ui::ALIGN_LEFT_CENTER,
//                .direction = ui::HORIZONTALLY,
//            }
//        };
//        _layout.styles( FileBrowserFilesContainer ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .padding = global_padding,
//            .height = ui::extend(),
//            .width = ui::extend(),
//            .gap = {10,10},
//            .border_radius = 5,
//            .clip_overflow = true,
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_CENTER,
//                .direction = ui::HORIZONTALLY,
//                .wrap_around = true,
//            }
//        };
//        _layout.styles( FileBrowserScrollContainer ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::extend(),
//            .height = ui::flexible(),
//            .padding = global_padding,
//            .gap = {10,10},
//            .border_radius = 5,
//            .alignment =
//            {
//                .origin = ui::ALIGN_TOP_LEFT,
//                .direction = ui::HORIZONTALLY,
//                .wrap_around = true,
//            }
//        };
//        _layout.styles( FileBG ) = ui::Style
//        {
//            .background = global_bgcolor,
//            .width = ui::flexible(),
//            .height = ui::flexible(),
//            .border_radius = 5,
//            .padding = {5,5,5,5},
//            .gap = {2,2},
//            .alignment =
//            {
//                .origin = ui::ALIGN_CENTER_CENTER,
//                .direction = ui::VERTICALLY,
//            },
//        };engine->graphic->
//        _layout.styles( FileIcon ) =  ui::Style
//        {
//            .background = ui::bgColor(0x141223FF),
//            .width = ui::fixed( 64 ),
//            .height = ui::fixed( 64 ),
//            .border_radius = 5,
//            .click_trigger = ui::ClickTrigger::OnClick,
//        };
//        _layout.styles( FileName ) = ui::Style
//        {
//            .background = ui::bgColor({1,1,1,0.1}),
//            .width = ui::fixed( 128 ),
//            .height = ui::fixed( 20 ),
//            .border_radius = 5,
//        };



        
//        _layout.styles( FPSCounter ) = ui::Style
//        {
//            .background = bgcolor,
//            .width = ui::fixed( 50 ),
//            .height = ui::fixed( 20 ),
//        };
//
//        _layout.styles( DragResizerX ) =
//        {
//            .background = {},
//            .width = ui::fixed( 10 ),
//            .height = ui::extend(),
//            .click_trigger = ui::ClickTrigger::OnClick
//        };
//
//        _layout.styles( DragResizerY ) =
//        {
//            .background = {},
//            .width = ui::extend(),
//            .height = ui::fixed( 10 ),
//            .click_trigger = ui::ClickTrigger::OnClick,
//        };
//
//
//
//
//        _layout.styles( HideFilesIcon ) =
//        {
//            .click_trigger = ui::ClickTrigger::OnRelease,
//            .background = global_bgcolor,
//            .width = ui::fixed( 32 ),
//            .height = ui::fixed( 32 ),
//            .padding = {4,4,4,4},
//            .gap = {2,2},
//            .border_radius = 5,
//            .alignment =
//            {
//                .origin = ui::ALIGN_CENTER_CENTER,
//                .direction = ui::VERTICALLY
//            },
//        };
//
//        _layout.styles( HideFilesIconRect1 ) =
//        {
//            .background = global_bgcolor,
//            .width = ui::extend(),
//            .height = ui::extend(),
//            .border_radius = 3,
//            .mouseover = false,
//        };
//        _layout.styles( HideFilesIconRect2 ) =
//        {
//            .background = ui::bgColor({1,1,1,0.5}),
//            .width = ui::extend(),
//            .height = ui::extend(),
//            .border_radius = 3,
//            .mouseover = false,
//        };
//
//
//
//
//        _layout.styles( HideNavBarIcon ) =
//        {
//            .click_trigger = ui::ClickTrigger::OnRelease,
//            .background = global_bgcolor,
//            .width = ui::fixed( 32 ),
//            .height = ui::fixed( 32 ),
//            .padding = {4,4,4,4},
//            .gap = {2,2},
//            .border_radius = 5,
//            .alignment =
//            {
//                .origin = ui::ALIGN_CENTER_CENTER,
//                .direction = ui::HORIZONTALLY
//            },
//        };
//
//        _layout.styles( HideNavBarIconRect1 ) =
//        {
//            .background = ui::bgColor({1,1,1,0.5}),
//            .width = ui::extend(),
//            .height = ui::extend(),
//            .border_radius = 3,
//            .mouseover = false,
//        };
//        _layout.styles( HideNavBarIconRect2 ) =
//        {
//            .background = global_bgcolor,
//            .width = ui::extend(),
//            .height = ui::extend(),
//            .border_radius = 3,
//            .mouseover = false,
//        };
//
//        _file_browser.current_path = "/Users/kae/Projects/game-engine-projects/";
//        populateFileBrowserContents( _file_browser );
    }
}
