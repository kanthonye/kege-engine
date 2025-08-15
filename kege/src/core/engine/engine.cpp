//
//  engine.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/27/25.
//

#include "../../core/engine/engine.hpp"
#include "deferred-render-graph-setup.hpp"

namespace kege{

    void Engine::addSystem( const std::string& name )
    {
        kege::Ref< kege::System > system = SystemFactory::instance().create( name, this );
        if ( system )
        {
            addSystem( system );
        }
        else
        {
            KEGE_LOG_ERROR << "system create function for -> "<< name <<" dont exist." <<Log::nl;
        }
    }

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
        for (kege::Ref< kege::System > system : _systems )
        {
            system->onSceneChange();
        }
    }

    kege::Scene* Engine::getScene()
    {
        return _scene.ref();
    }

    void Engine::addSystem( kege::Ref< kege::System > system )
    {
        _systems.push_back( system );

        if ( system->checkFlag( kege::System::REQUIRE_UPDATE ) )
        {
            _system_updates.push_back( system.ref() );
        }
        if ( system->checkFlag( kege::System::REQUIRE_RENDER ) )
        {
            _system_renders.push_back( system.ref() );
        }
        if ( system->checkFlag( kege::System::REQUIRE_INPUT ) )
        {
            _system_inputs.push_back( system.ref() );
        }
    }

    kege::InputContextManager& Engine::getInputContextManager()
    {
        return _input_context_manager;
    }

    kege::UserInputReceiver& Engine::getUserInputReceiver()
    {
        return _input_receiver;
    }

    kege::MappedInputs& Engine::getMappedInputs()
    {
        return _mapped_inputs;
    }

    kege::AssetSystem& Engine::getAssetSystem()
    {
        return _asset_system;
    }

    kege::VirtualDirectory& Engine::getVirtualDirectory()
    {
        return _virtual_directory;
    }
    
    kege::RenderGraph* Engine::getRenderGraph()
    {
        return _graph.ref();
    }

    kege::GraphicsWindow* Engine::getWindow()
    {
        return _window.ref();
    }
    
    kege::Graphics* Engine::getGraphics()
    {
        return _graphics.ref();
    }

    bool Engine::initalize()
    {
        Log::info << "\n- Engine initializing...\n";

        _root_directory = "/Users/kae/Developer/xcode/KE-GE/kege";
        _virtual_directory.add( "assets", "/Users/kae/Developer/xcode/KE-GE/kege/assets" );
        _virtual_directory.add( "graphics-shaders", "/Users/kae/Developer/xcode/KE-GE/kege/assets/shaders/glsl/graphics" );
        _virtual_directory.add( "compute-shaders", "/Users/kae/Developer/xcode/KE-GE/kege/assets/shaders/glsl/compute" );

        if ( !initalizeAllSystems() )
        {
            return false;
        };

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
        for (kege::Ref< kege::System > system : _systems )
        {
            system->onSceneChange();
        }

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

        _input_context_manager.shutdown();
        _input_receiver.shutdown();
        _mapped_inputs.clear();

        _system_inputs.clear();
        _system_renders.clear();
        _system_updates.clear();
        shutdownAllSystems();

        //if ( _audio_device )
        //{
        //    _audio_device->shutdown();
        //    _audio_device.clear();
        //}

        if ( _graph )
        {
            _graph.clear();
        }

        if ( _graphics )
        {
            _graphics->shutdown();
            _graphics.clear();
        }

        if ( _window )
        {
            _window->destroy();
            _window.clear();
        }
    }

    void Engine::render()
    {
        if ( _graphics->beginFrame() < 0 )
        {
            KEGE_LOG_ERROR << "Failed to begin Frame" <<Log::nl;
            _running = false;
        }

        _graph->execute();

        if ( !_graphics->endFrame() )
        {
            KEGE_LOG_ERROR << "Failed to end Frame" <<Log::nl;
            _running = false;
        }
    }

    void Engine::update()
    {
        if ( !_scene->ready() )
        {
            _scene->initialize();
        }

        //while ( _lag >= _fixed_delta_time )
        {
            for (kege::System* system : _system_updates )
            {
                system->update( _fixed_delta_time );
            }
            _lag -= _fixed_delta_time;
        }
    }

    void Engine::input()
    {
        std::vector< kege::Input > inputs;
        _input_receiver.getInputs( inputs );

        //if ( _gui.process( inputs ) ) return;

        _input_context_manager.process( inputs );
        Communication::broadcast< const MappedInputs& >( _mapped_inputs );
//        for ( kege::System* system : _system_inputs )
//        {
//            system->input( inputs );
//        }
    }

    double Engine::calcDeltaTime()
    {
        Duration current_time = now();
        // Calculate elapsed time
        double dms = std::chrono::duration_cast< std::chrono::milliseconds >( current_time - _previous_time ).count() / 1000.0;
        _previous_time = current_time;
        return dms;
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

        while ( _running && !_window->shouldClose() )
        {
            try
            {
                _window->pollEvents();

                tick();
                input();
                update();
                render();
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

    bool Engine::initalizeAllSystems()
    {
        for (kege::Ref< kege::System >& system : _systems )
        {
            if ( !system->initialize() )
            {
                return false;
            }
        }
        return true;
    }

    void Engine::shutdownAllSystems()
    {
        std::vector< kege::Ref< kege::System > >::reverse_iterator syst;
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
    {
        _start_time = std::chrono::high_resolution_clock::now();

        addSystem( "logging" );
        addSystem( "graphics" );
        addSystem( "render-graph" );
        addSystem( "input" );
        addSystem( "ecs" );

        addSystem( "camera-controller" );
        addSystem( "lookat" );
        addSystem( "follow" );
        addSystem( "movement-controller" );
        
        addSystem( "compute-camera-ray" );
        addSystem( "entity-selecter" );
        addSystem( "entity-dragging" );

        addSystem( "particle-emitter-updater" );
        addSystem( "particle-effect-updater" );

        addSystem( "physics" );
        addSystem( "rigidbody-to-transform" );

        addSystem( "camera" );
        addSystem( "mesh-rendering" );
        
        addSystem( "entity-factory" );

//

        //_engine.add< kege::SleepSystem >();
//        addSystem< kege::LoggingSystem >();
//        addSystem< kege::GraphicsSystem >();
//        addSystem< kege::RenderingSystem >();
//        addSystem< kege::MeshRenderingSystem >();
//        addSystem< kege::DebugLineRenderSystem >();
//        addSystem< kege::ECSystem >();
//        addSystem< kege::InputSystem >();
//        addSystem< kege::CameraSystem >();
//        addSystem< kege::LookAtSystem >();
//        addSystem< kege::FollowSystem >();
//        addSystem< kege::MovementSystem >();
//        addSystem< kege::ComputeSceneRay >();
//
//        addSystem< kege::ParticleEmissionSystem >();
//        addSystem< kege::ParticleSystem >();
//        addSystem< kege::BillboardParticleRenderer >();
//
//        addSystem< EntityFactory >();
//        addSystem< kege::EntitySelectionSystem >();
//        addSystem< kege::EntityDraggingSystem >();
//
//        addSystem< kege::UpdateDecayOverTime >();
//        addSystem< kege::PhysicsSystem >();
//        //_engine.add< kege::SleepSystem >();
    }

}
