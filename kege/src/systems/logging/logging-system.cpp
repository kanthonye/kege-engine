//
//  logging-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/15/25.
//

#include "logging-system.hpp"

namespace kege{

    LoggingSystem::LoggingSystem( kege::Engine* engine )
    :   System( engine, "logging-system", 0 )
    {}

    bool LoggingSystem::initialize()
    {
        //Logger::get().open( "/Users/kae/Projects/game-engine-projects/kege/kege/log/errors.log" );
        return System::initialize();
    }

    void LoggingSystem::shutdown()
    {
        System::shutdown();
    }

    KEGE_REGISTER_SYSTEM( LoggingSystem, "logging" );
}
