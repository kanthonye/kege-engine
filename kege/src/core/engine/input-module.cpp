//
//  input-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#include "engine.hpp"
#include "input-module.hpp"

namespace kege{

    bool InputModule::initialize()
    {
        if ( _module != nullptr )
        {
            kege::Log::error << "InputModule already initialized!" << Log::nl;
            return false;
        }

        kege::Log::info << "initializing -> " << getName() << Log::nl;

        _module = new kege::InputContextManager;
        if( !_module->initialize( _engine->graphics()->getWindow() ) )
        {
            KEGE_LOG_ERROR << getName() << " initialization Failed.";
            return false;
        }

        string input_file = _engine->vfs()->fetch( "assets/config/keybinds.json" );
        kege::Ref< kege::InputContext > context = kege::InputContextLoader::load( input_file );
        if ( !context )
        {
            KEGE_LOG_ERROR << getName() << " failed to load input config -> " << input_file <<"\n";
            return false;
        }

        _module->addContext( "keybinds", context );
        return true;
    }

    void InputModule::shutdown()
    {
        if ( _module )
        {
            _module->shutdown();
            _module.clear();
        }
    }

    void InputModule::add()
    {
        _engine->addModule( this );
        kege::Log::info << "InputModule module added to engine" << Log::nl;
    }

    InputModule::InputModule( kege::Engine* engine )
    :   Module( engine, "InputModule" )
    {
    }

}
