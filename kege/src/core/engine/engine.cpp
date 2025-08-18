//
//  engine.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/27/25.
//

#include "../../core/engine/engine.hpp"

namespace kege{

//    void Engine::addSystem( const std::string& name )
//    {
//        kege::Ref< kege::System > system = SystemFactory::instance().create( name, this );
//        if ( system )
//        {
//            addSystem( system );
//        }
//        else
//        {
//            KEGE_LOG_ERROR << "system create function for -> "<< name <<" dont exist." <<Log::nl;
//        }
//    }


    void Engine::addModule( kege::Module* module )
    {
        _modules.push_back( module );
    }

//    kege::InputContextManager& Engine::getInputContextManager()
//    {
//        return _input_context_manager;
//    }
//
//    kege::UserInputReceiver& Engine::getUserInputReceiver()
//    {
//        return _input_receiver;
//    }
//
//    kege::MappedInputs& Engine::getMappedInputs()
//    {
//        return _mapped_inputs;
//    }

    kege::AssetSystem& Engine::assetSystem()
    {
        return _asset_system;
    }

    kege::CoreRenderGraph& Engine::renderGraph()
    {
        return _render_graph;
    }
    
    kege::GraphicsModule& Engine::graphics()
    {
        return _graphics;
    }

    kege::InputModule& Engine::input()
    {
        return _input;
    }

    kege::EntitySystemManagerModule& Engine::esm()
    {
        return _esm;
    }

    kege::ECSModule& Engine::ecs()
    {
        return _ecs;
    }

    kege::VirtualDirectoryModule& Engine::vfs()
    {
        return _vfs;
    }

    kege::LoggerModule& Engine::logger()
    {
        return _logger;
    }

    kege::SceneModule& Engine::scene()
    {
        return _scene;
    }
//    kege::RenderGraph* Engine::getRenderGraph()
//
//    kege::GraphicsWindow* Engine::getWindow()
//    {
//        return _window.ref();
//    }
//    
//    kege::Graphics* Engine::getGraphics()
//    {
//        return _graphics._module.;
//    }

    bool Engine::initialize()
    {
        _root_directory = "/Users/kae/Developer/xcode/KE-GE/kege";

        if ( !initalizeCoreSystems() )
        {
            return false;
        }

        return true;
    }

    void Engine::shutdown()
    {
        _asset_system.shutdown();
        shutdownCoreSystems();
    }

    double Engine::calcDeltaTime()
    {
        Duration current_time = now();
        // Calculate elapsed time
        double dms = std::chrono::duration_cast< std::chrono::milliseconds >( current_time - _previous_time ).count() / 1000.0;
        _previous_time = current_time;
        return dms;
    }

    double Engine::dms()const
    {
        return _fixed_delta_time;
    }
    
    void Engine::tick()
    {
        _delta_time = calcDeltaTime();
        // Clamp deltaTime to avoid spiral of death
        if (_delta_time > _max_frame_time)
        {
            _delta_time = _max_frame_time;
        }

        _lag += _delta_time;
    }

    void Engine::run()
    {
        _running = true;
        tick();

        while ( _running && !_graphics->windowIsOpen() )
        {
            try
            {
                _graphics->getWindow()->pollEvents();

                tick();
                _input->updateCurrentInputs();

                if ( !_scene->ready() )
                {
                    _scene->initialize();
                }
                //while ( _lag >= _fixed_delta_time )
                {
                    _esm->update( _fixed_delta_time );
                    _lag -= _fixed_delta_time;
                }


                if ( 0 <= _graphics->beginFrame() )
                {
                    _render_graph->execute();
                    if ( !_graphics->endFrame() )
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
            }
            catch ( const std::exception& arg )
            {
                KEGE_LOG_ERROR << arg.what() <<Log::nl;
                _running = false;
            }
        }
    }

    Duration Engine::now()
    {
        return std::chrono::high_resolution_clock::now() - _start_time;
    }

    bool Engine::initalizeCoreSystems()
    {
        _previous_time = Duration::zero(); // Reset for first delta time calculation in loop()
        _lag = 0.0;

        Log::info << "engine initializing " << Log::nl;
        for ( kege::Module* system : _modules )
        {
            Log::info << "initializing -> " << system->getName() <<"\n";
            if ( !system->initialize() )
            {
                Log::error  << system->getName()<< " -> initialization failed" <<Log::nl;
                return false;
            }
            Log::info  << system->getName()<< " -> initialization complete \n";
        }
        Log::info << "engine initialization complete..." << Log::nl;
        return true;
    }

    void Engine::shutdownCoreSystems()
    {
        std::vector< kege::Module* >::reverse_iterator syst;
        for ( syst = _modules.rbegin(); syst != _modules.rend(); syst++ )
        {
            Log::info << "shuting-down -> " << (*syst)->getName() <<"\n";
            (*syst)->shutdown();
            Log::info  << "successfully shutdown -> " << (*syst)->getName() <<"\n";
        }
        _modules.clear();
    }

    Engine::~Engine()
    {
        shutdown();
    }

    Engine::Engine()
    :   _previous_time( Duration::zero() )
    ,   _fixed_delta_time(  1.0 / 60.0  )
    ,   _max_frame_time( 0.25 )
    ,   _delta_time( 0 )
    ,   _lag( 0 )
    ,   _running( false )
    ,   _start_time( std::chrono::high_resolution_clock::now() )
    ,   _graphics( this )
    ,   _render_graph( this )
    ,   _input( this )
    ,   _esm( this )
    ,   _ecs( this )
    ,   _vfs( this )
    ,   _logger( this )
    ,   _scene( this )
    ,   _modules()
    ,   _root_directory( "" )
    {
        _start_time = std::chrono::high_resolution_clock::now();
    }

}
