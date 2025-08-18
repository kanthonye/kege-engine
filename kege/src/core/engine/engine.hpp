//
//  engine.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/27/25.
//

#ifndef engine_hpp
#define engine_hpp

#include "../utils/log.hpp"
#include "../system/system.hpp"
#include "../scene/scene.hpp"
#include "../scene/scene-loader.hpp"

#include "core-ecs.hpp"
#include "core-esm.hpp"
#include "core-vfs.hpp"
#include "input-module.hpp"
#include "scene-module.hpp"
#include "core-graphics.hpp"
#include "core-render-graph.hpp"
#include "logger-module.hpp"

namespace kege{

    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using Duration = std::chrono::high_resolution_clock::duration;

    class Engine
    {
    public:

        void addModule( kege::Module* module );


        kege::AssetSystem& assetSystem();

        kege::CoreRenderGraph& renderGraph();
        kege::GraphicsModule& graphics();
        kege::InputModule& input();
        kege::EntitySystemManagerModule& esm();
        kege::ECSModule& ecs();
        kege::VirtualDirectoryModule& vfs();
        kege::LoggerModule& logger();
        kege::SceneModule& scene();

        double dms()const;

        bool initialize();
        void shutdown();
        void tick();
        void run();

        ~Engine();
        Engine();

    private:

        void addSystem( kege::System* system );

        bool initalizeCoreSystems();
        void shutdownCoreSystems();

        double calcDeltaTime();
        Duration now();

    private:

        kege::LoggerModule _logger;
        kege::GraphicsModule _graphics;
        kege::CoreRenderGraph _render_graph;
        kege::InputModule _input;
        kege::EntitySystemManagerModule _esm;
        kege::ECSModule _ecs;
        kege::VirtualDirectoryModule _vfs;
        kege::SceneModule _scene;

        kege::AssetSystem _asset_system;

        std::vector< kege::Module* > _modules;

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
