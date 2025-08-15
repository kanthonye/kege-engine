//
//  engine.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/27/25.
//

#include "../../core/engine/engine.hpp"
#include "deferred-render-graph-setup.hpp"

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

    void Engine::changeScene( uint32_t scene_id )
    {
        if ( _scene_files.size() <= scene_id )
        {
            KEGE_LOG_ERROR << "INVALID_SCENE_INDEX : scene index out of bound." <<Log::nl;
            return;
        }

        kege::Ref< kege::Scene > scene = SceneLoader::load( _scene_files[ scene_id ] );
        if ( scene == nullptr )
        {
            KEGE_LOG_ERROR << "LOADING_FAILED : loadScene -> " << _scene_files[ scene_id ] <<Log::nl;
            return;
        }

        setScene( scene );
    }

    void Engine::setScene( kege::Ref< kege::Scene > scene )
    {
        if ( _scene )
        {
            _scene->shutdown();
        }
        _scene = scene;

        // alert systems of the scene change
        _esm->onSceneChange();
    }

    kege::Scene* Engine::getScene()
    {
        return _scene.ref();
    }

    void Engine::addSystem( kege::System* system )
    {
        _systems.push_back( system );
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

    kege::VirtualDirectory& Engine::getVirtualDirectory()
    {
        return _virtual_directory;
    }

    kege::CoreRenderGraph& Engine::renderGraph()
    {
        return _render_graph;
    }
    kege::CoreGraphics& Engine::graphics()
    {
        return _graphics;
    }
    kege::CoreInput& Engine::input()
    {
        return _input;
    }
    kege::CoreESM& Engine::esm()
    {
        return _esm;
    }
    kege::CoreECS& Engine::ecs()
    {
        return _ecs;
    }
    kege::CoreVFS& Engine::vfs()
    {
        return _vfs;
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

        // 10. Load Initial Scene
        // Example: Load the first scene file path if available
        if ( !_scene_files.empty() )
        {
            _scene = SceneLoader::load( _scene_files[0] );
        }
        else
        {
             _scene = kege::Ref< kege::Scene >( new kege::Scene() );
        }
        if ( _scene ) _scene->initialize(); // Initialize the newly created scene
        if ( !_scene || !_scene->ready() )
        {
            KEGE_LOG_ERROR << "Failed to load or initialize initial scene." << kege::Log::nl;
            return false;
        }
        Log::info << "- Scene initalized...\n";


        // alert systems of the scene change
        _esm->onSceneChange();

        // 12. Final Preparations
        _previous_time = Duration::zero(); // Reset for first delta time calculation in loop()
        _lag = 0.0;

        return true;
    }

    void Engine::shutdown()
    {
        if ( _scene )
        {
            _scene->shutdown();
            _scene.clear();
        }
        _scene_files.clear();

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
                _input.update();

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
        Log::info << "engine initializing " << Log::nl;
        for ( kege::System* system : _systems )
        {
            Log::info << "initializing -> " << system->getName() <<"\n";
            if ( !system->initialize() )
            {
                Log::info  << system->getName()<< " -> initialization failed\n";
                return false;
            }
            Log::info  << system->getName()<< " -> initialization complete \n";
        }
        Log::info << "engine initialization complete..." << Log::nl;
        return true;
    }

    void Engine::shutdownCoreSystems()
    {
        std::vector<  kege::System* >::reverse_iterator syst;
        for ( syst = _systems.rbegin(); syst != _systems.rend(); syst++ )
        {
            Log::info << "shuting-down -> " << (*syst)->getName() <<"\n";
            (*syst)->shutdown();
            Log::info  << "successfully shutdown -> " << (*syst)->getName() <<"\n";
        }
        _systems.clear();
    }

    Engine::~Engine()
    {
        shutdown();
    }

    Engine::Engine()
    :   _scene( nullptr )
    ,   _previous_time( 0 )
    ,   _fixed_delta_time(  1.0 / 60.0  )
    ,   _max_frame_time( 0.25 )
    ,   _delta_time( 0 )
    ,   _lag( 0 )
    ,   _ecs( this )
    ,   _esm( this )
    ,   _graphics( this )
    ,   _input( this )
    ,   _render_graph( this )
    ,   _vfs( this )
    {
        _start_time = std::chrono::high_resolution_clock::now();

        addSystem( &_vfs );
        addSystem( &_graphics );
        addSystem( &_render_graph );
        addSystem( &_input );
        addSystem( &_ecs );
        addSystem( &_esm );
    }

}
