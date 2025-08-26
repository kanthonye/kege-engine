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


        _layout.addStyle
        ({
            "main",
            {
                .background = ui::bgColor(0x000000FF),
                .width = ui::fixed( _engine.graphics()->windowWidth() ),
                .height = ui::fixed( _engine.graphics()->windowHeight() ),
                .padding = {4,4,4,4},
                .gap = {2,2},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            }
        });

        _layout.addStyle
        ({
            "content",
            {
                .position = ui::Positioning::Relative,
                .background = ui::bgColor(0xFFFFFF0B),
                .width = ui::extend(),
                .height = ui::extend(),
                .gap = {2,2},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            },
        });
        _layout.addStyle
        ({
            "side-panel",
            {
                .background = ui::bgColor(0xFFFFFF0B),
                .position = ui::Positioning::Relative,
                .width = ui::fixed( 350 ),
                .height = ui::extend(),
                .gap = {2,2},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            },
        });

        _layout.addStyle
        ({
            "viewport",
            {
                .background = ui::bgColor(0x171717FF),
                .width = ui::extend(),
                .height = ui::extend(),
            },
        });

        _layout.addStyle
        ({
            "navbar",
            {
                .background = ui::bgColor(0xFFFFFF13),
                .width = ui::extend(),
                .height = ui::fixed( 30 ),
                .gap = {2,2},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            }
        });
        _layout.addStyle
        ({
            "inspector",
            {
                .background = ui::bgColor(0xFFFFFF13),
                .width = ui::extend(),
                .height = ui::extend(),
                .padding = {},
                .gap = {3,3},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            }
        });
        _layout.addStyle
        ({
            "hierarchy",
            {
                .background = ui::bgColor(0xFFFFFF13),
                .width = ui::extend(),
                .height = ui::extend(),
                .padding = {},
                .gap = {3,3},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            },
        });



        _layout.addStyle
        ({
            "droplist",
            {
                .width = ui::extend(),
                .height = ui::flexible(),
                .background = ui::bgColor(0xFFFFFF00),
                .color = ui::rgba(0xFFFFFF83),
                .align =
                {
                    .origin = ui::ALIGN_TOP_LEFT,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            }
        });
        _layout.addStyle
        ({
            "droplist-content",
            {
                .width = ui::extend(),
                .height = ui::flexible(),
                .background = ui::bgColor(0xFFFFFF00),
                .gap = {10,1},
                .align =
                {
                    .origin = ui::ALIGN_TOP_LEFT,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            }
        });
        _layout.addStyle
        ({
            "droplist-field",
            {
                .font_size = 20,
                .width = ui::extend(),
                .height = ui::fixed(20),
                .background = ui::bgColor(0xFFFFFF0B),
                .color = ui::rgba(0xFFFFFF83),
                .wrap_around = false,
                .align =
                {
                    .origin = ui::ALIGN_LEFT_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            },
        });
        _layout.addStyle
        ({
            "droplist-field-highlight",
            {
                .font_size = 20,
                .width = ui::extend(),
                .height = ui::fixed(20),
                .background = ui::bgColor(0xFFFFFF20),
                .color = ui::rgba(0xFFFFFF83),
                .wrap_around = false,
                .align =
                {
                    .origin = ui::ALIGN_LEFT_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            },
        });
        _layout.addStyle
        ({
            "droplist-icon",
            {
                .border_radius = 5,
                .width = ui::fixed(16),
                .height = ui::extend(),
                .background = ui::bgColor(0xFFFFFF00),
                .color = ui::rgba(0xFFFFFF83),
                .padding = {5,0,0,0},
            },
        });
        _layout.addStyle
        ({
            "droplist-label",
            {
                .font_size = 20,
                .width = ui::extend(),
                .height = ui::fixed(20),
                .background = ui::bgColor(0xFFFFFF00),
                .color = ui::rgba(0xFFFFFF83),
                .padding = {0,0,0,0},
            },
        });



        _layout.addStyle
        ({
            "properties-container",
            {
                .width = ui::extend(),
                .height = ui::flexible(),
                .background = ui::bgColor(0xFFFFFF0B),
                .color = ui::rgba(0xFFFFFF83),
                .wrap_around = false,
                .align =
                {
                    .origin = ui::ALIGN_TOP_LEFT,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            }
        });
        _layout.addStyle
        ({
            "properties-content",
            {
                .width = ui::extend(),
                .height = ui::flexible(),
                .background = ui::bgColor(0xFFFFFF00),
                .padding = {10,2,10,10},
                .gap = {2,2},
                .align =
                {
                    .origin = ui::ALIGN_TOP_LEFT,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            }
        });
        _layout.addStyle
        ({
            "properties-field",
            {
                .font_size = 20,
                .padding = {10,4,4,4},
                .width = ui::extend(),
                .height = ui::fixed(30),
                .background = ui::bgColor(0xFFFFFF00),
                .color = ui::rgba(0xFFFFFF83),
                .align =
                {
                    .origin = ui::ALIGN_LEFT_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            }
        });
        _layout.addStyle
        ({
            "properties-icon",
            {
                .border_radius = 5,
                .width = ui::fixed(16),
                .height = ui::extend(),
                .background = ui::bgColor(0xFFFFFF00),
                .color = ui::rgba(0xFFFFFF83),
                .padding = {0,0,0,0},
                .align =
                {
                    .origin = ui::ALIGN_CENTER_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            }
        });
        _layout.addStyle
        ({
            "properties-label",
            {
                .font_size = 20,
                .width = ui::extend(),
                .height = ui::fixed(20),
                .background = ui::bgColor(0xFFFFFF00),
                .color = ui::rgba(0xFFFFFF83),
            }
        });



        _layout.addStyle
        ({
            "numeric-container",
            {
                .background = ui::bgColor(0xFFFFFF0B),
                .border_radius = 5,
                .width = ui::extend(),
                .height = ui::flexible(),
                .padding = {8,4,8,4},
                .gap.width = 2,
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            }
        });
        _layout.addStyle
        ({
            "numeric-label",
            {
                .background = ui::bgColor(0xFFFFFF00),
                .width = ui::fixed(20),
                .height = ui::fixed(20),
                .color = ui::rgb(0x9C9C9C),
                .padding = {0,0,0,0},
                .align_text = ui::AlignText::Left,
                .font_size = 20,
            }
        });
        _layout.addStyle
        ({
            "numeric-value",
            {
                .background = ui::bgColor(0xFFFFFF00),
                .width = ui::extend(),
                .height = ui::fixed(20),
                .color = ui::rgb(0x9C9C9C),
                .padding = {0,0,0,0},
                .border_radius = 5,
                .align_text = ui::AlignText::Right,
                .font_size = 20,
            }
        });
        _layout.addStyle
        ({
            "numeric-focus",
            {
                .background = ui::bgColor(0xFFFFFF1B),
                .border_radius = 5,
                .width = ui::extend(),
                .height = ui::flexible(),
                .padding = {8,4,8,4},
                .gap.width = 2,
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            }
        });



        _layout.addStyle
        ({
            "vector-container",
            {
                .background = ui::bgColor(0x1E1E1EFF),
                .width = ui::extend(),
                .height = ui::flexible(),
                .color = ui::rgb(0x9C9C9C),
                .border_radius = 5,
                .padding = {10,5,10,10},
                .gap = {2,5},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            },
        });
        _layout.addStyle
        ({
            "vector-content",
            {
                .background = ui::bgColor(0x1E1E1E00),
                .width = ui::extend(),
                .height = ui::flexible(),
                .color = ui::rgb(0x9C9C9C),
                .border_radius = 5,
                .gap = {3,3},
                .align =
                {
                    .origin = ui::ALIGN_LEFT_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            },
        });
        _layout.addStyle
        ({
            "vector-label",
            {
                .background = ui::bgColor(0xFFFFFF00),
                .width = ui::extend(),
                .height = ui::fixed(20),
                .color = ui::rgb(0x9C9C9C),
                .padding = {0,2,0,0},
                .font_size = 20,
                .align_text = ui::AlignText::Left,
            }
        });




        _layout.addStyle
        ({
            "menu-field",
            {
                .font_size = 20,
                .padding = {4,4,4,4},
                .width = ui::fixed( 100 ),
                .height = ui::fixed(30),
                .background = ui::bgColor(0xFFFFFF0B),
                .color = ui::rgba(0xFFFFFF83),
                .align =
                {
                    .origin = ui::ALIGN_LEFT_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            }
        });
        _layout.addStyle
        ({
            "menu-content",
            {
                .width = ui::fixed( 300 ),
                .height = ui::flexible(),
                .background = ui::bgColor(0xFFFFFF0B),
                .padding = {10,2,10,10},
                .gap = {2,2},
                .align =
                {
                    .origin = ui::ALIGN_TOP_LEFT,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            },
        });


        _layout.addStyle
        ({
            "option-field",
            {
                .font_size = 20,
                .padding = {4,4,4,4},
                .width = ui::fixed( 100 ),
                .height = ui::fixed(30),
                .background = ui::bgColor(0xFFFFFF0B),
                .color = ui::rgba(0xFFFFFF83),
                .align =
                {
                    .origin = ui::ALIGN_LEFT_CENTER,
                    .direction = ui::DIRECTION_LEFT_TO_RIGHT,
                },
            }
        });
        _layout.addStyle
        ({
            "option-content",
            {
                .position = ui::Positioning::Absolute,
                .width = ui::fixed( 300 ),
                .height = ui::flexible(),
                .background = ui::bgColor(0xFFFFFF0B),
                .padding = {10,2,10,10},
                .gap = {2,2},
                .zindex = 10,
                .align =
                {
                    .origin = ui::ALIGN_TOP_LEFT,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                },
            }
        });
        _layout.addStyle
        ({
            "option-item",
            {
                .font_size = 20,
                .padding = {4,2,4,2},
                .width = ui::extend(),
                .height = ui::fixed(25),
                .background = ui::bgColor(0xFFFFFF00),
                .color = ui::rgba(0xFFFFFF83),
            }
        });

        
        main_panel = _layout.make({
            .style = _layout.getStyleByName( "main")
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

            _input.processInputs( _engine.input()->getCurrentInputs() );

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

            _layout.push( _layout.make({ .visible = false, .style = _layout.getStyleByName( "content" ) }) );
            {
                _viewport_panel.put( _layout );

                _layout.push( _layout.make({ .visible = false, .style = _layout.getStyleByName( "side-panel" ) }) );
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
        _viewer.begin( encoder );
        _viewer.draw( _layout, 1, _layout[1]->rect );
        _viewer.end();
    }

    Editor::Editor()
    :   _paused( false )
    {
    }
}
