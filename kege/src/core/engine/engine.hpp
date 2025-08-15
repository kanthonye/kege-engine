//
//  engine.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/27/25.
//

#ifndef engine_hpp
#define engine_hpp

#include "log.hpp"
#include "virtual-directory.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "scene-loader.hpp"
#include "user-input-receiver.hpp"

#include "../../core/container/asset-system.hpp"
#include "../../core/system/system-factory.hpp"
#include "../../core/graphics/core/graphics.hpp"
#include "../../core/graphics/graph/render-graph.hpp"
#include "../../core/input/input-context-manager.hpp"

namespace kege{

    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using Duration = std::chrono::high_resolution_clock::duration;

    class Engine
    {
    public:

        template< typename SystemT > SystemT* addSystem()
        {
            SystemT* system = new SystemT( this );
            insert( system );
            return system;
        }

        void addSystem( const std::string& name );

        void changeScene( uint32_t scene_id );
        void setScene( kege::Ref< kege::Scene > scene );
        kege::Scene* getScene();



        kege::InputContextManager& getInputContextManager();
        kege::UserInputReceiver& getUserInputReceiver();
        kege::MappedInputs& getMappedInputs();
        kege::AssetSystem& getAssetSystem();

        kege::VirtualDirectory& getVirtualDirectory();
        kege::RenderGraph* getRenderGraph();
        kege::GraphicsWindow* getWindow();
        kege::Graphics* getGraphics();

        bool initalize();
        void shutdown();
        void run();

        ~Engine();
        Engine();

    private:

        void addSystem( kege::Ref< kege::System > system );
        void update();
        void render();
        void input();
        void tick();


        bool initalizeAllSystems();
        void shutdownAllSystems();

        double calcDeltaTime();
        Duration now();

    private:

        SystemFactory _system_factory;
        //kege::Ref< kege::audio::Device > _audio_device;

        kege::Ref< kege::GraphicsWindow > _window;
        kege::Ref< kege::Graphics > _graphics;

        kege::Ref< kege::RenderGraph > _graph;

        kege::AssetSystem _asset_system;

        /**
         * Store the current input comming in the user. Note: These inputs must me cleared every frame.
         */
        kege::UserInputReceiver _input_receiver;
        kege::InputContextManager _input_context_manager;
        kege::MappedInputs _mapped_inputs;

        kege::VirtualDirectory _virtual_directory;


        std::vector< kege::Ref< kege::System > > _systems;

        /**
         * systems that require their update function to be called
         */
        std::vector< kege::System* > _system_updates;

        /**
         * systems that requires their render function to be called
         */
        std::vector< kege::System* > _system_renders;

        /**
         * systems that requires their input function to be called
         */
        std::vector< kege::System* > _system_inputs;

        std::vector< std::string > _scene_files;
        kege::Ref< kege::Scene > _scene;

        std::string _root_directory;

        /**
         * Hold the time of this object creation. purpose is to get the difference
         * form the current time so that the now() function returns a time starting
         * from zero to the end of this objects life.
         */
        TimePoint _start_time;

        Duration _previous_time;
        double _fixed_delta_time;
        double _max_frame_time;
        double _delta_time;
        double _lag;

        bool _running;

        friend System;
    };

}
#endif /* engine_hpp */
