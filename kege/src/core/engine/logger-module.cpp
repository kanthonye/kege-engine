//
//  logger-module.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/17/25.
//

#include "engine.hpp"
#include "logger-module.hpp"

namespace kege{

    LoggerModule::LoggerModule( kege::Engine* engine )
    :   kege::Module( engine, "logger-module" )
    {}

    bool LoggerModule::initialize()
    {
        //Logger::get().open( "/Users/kae/Projects/game-engine-projects/kege/kege/log/errors.log" );
        return true;
    }

    void LoggerModule::shutdown()
    {
    }

    void LoggerModule::add()
    {
        _engine->addModule( this );
        kege::Log::info << "LoggerModule added to engine" << Log::nl;
    }

}
