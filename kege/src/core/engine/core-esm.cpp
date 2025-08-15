//
//  core-esm.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/14/25.
//

#include "../engine/core-esm.hpp"


namespace kege{

    CoreESM::CoreESM( kege::Engine* engine )
    :   kege::CoreSystem< kege::EntitySystemManager >( engine, "core-esm" )
    {}

    bool CoreESM::initialize()
    {
        _module = new kege::EntitySystemManager( _engine );

        _module->addSystem( "camera-controller" );
        _module->addSystem( "lookat" );
        _module->addSystem( "follow" );
        _module->addSystem( "movement-controller" );

        _module->addSystem( "compute-camera-ray" );
        _module->addSystem( "entity-selecter" );
        _module->addSystem( "entity-dragging" );

        _module->addSystem( "particle-emitter-updater" );
        _module->addSystem( "particle-effect-updater" );

        _module->addSystem( "physics" );
        _module->addSystem( "rigidbody-to-transform" );

        _module->addSystem( "camera" );
        _module->addSystem( "mesh-rendering" );

        _module->addSystem( "entity-factory" );

        if ( !_module->initialize() )
        {
            return false;
        }
        return true;
    }

    void CoreESM::shutdown()
    {
        if ( _module )
        {
            _module->shutdown();
            _module.clear();
        }
    }

    //KEGE_REGISTER_SYSTEM( CoreECS, "ecs" );
}
