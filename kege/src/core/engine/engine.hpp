//
//  engine.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/27/25.
//

#ifndef engine_hpp
#define engine_hpp

#include "log.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "scene-loader.hpp"
#include "user-input-receiver.hpp"

#include "../../core/container/asset-system.hpp"
#include "../../core/system/entity-system-manager.hpp"
#include "../../core/graphics/core/graphics.hpp"
#include "../../core/graphics/graph/render-graph.hpp"
#include "../../core/input/input-context-manager.hpp"

#include "core-ecs.hpp"
#include "core-esm.hpp"
#include "core-vfs.hpp"
#include "core-input.hpp"
#include "core-logging.hpp"
#include "core-graphics.hpp"
#include "core-render-graph.hpp"

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

        //void addSystem( const std::string& name );

        void changeScene( uint32_t scene_id );
        void setScene( kege::Ref< kege::Scene > scene );
        kege::Scene* getScene();

        kege::AssetSystem& assetSystem();

        kege::VirtualDirectory& getVirtualDirectory();
        kege::CoreRenderGraph& renderGraph();
        kege::CoreGraphics& graphics();
        kege::CoreInput& input();
        kege::CoreESM& esm();
        kege::CoreECS& ecs();
        kege::CoreVFS& vfs();

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

        kege::VirtualDirectory _virtual_directory;
        kege::CoreGraphics _graphics;
        kege::CoreRenderGraph _render_graph;
        kege::CoreInput _input;
        kege::CoreESM _esm;
        kege::CoreECS _ecs;
        kege::CoreVFS _vfs;
        
        kege::AssetSystem _asset_system;

        std::vector< kege::System* > _systems;

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
