//
//  input-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#include "../../core/engine/engine.hpp"
#include "input-system.hpp"

namespace kege{

    bool InputSystem::initialize()
    {
        if( !_engine->getUserInputReceiver().initialize( _engine->getWindow() ) )
        {
            KEGE_LOG_ERROR << "Failed to initialize SystemInput.";
            return false;
        }
        string input_file = _engine->getVirtualDirectory().fetch( "assets/config/keybinds.json" );
        kege::Ref< kege::InputContext > context = kege::InputContextLoader::load( input_file );
        if ( !context )
        {
            KEGE_LOG_ERROR << "InputContext::load. " << input_file;
            return false;
        }
        _engine->getInputContextManager().setMappedInputs( &_engine->getMappedInputs() );
        _engine->getInputContextManager().addContext( "keybinds", context );
        return System::initialize();
    }

    void InputSystem::shutdown()
    {
        System::shutdown();
    }

    InputSystem::InputSystem( kege::Engine* engine )
    :   kege::System( engine, "input-system", 0 )
    {
    }

    KEGE_REGISTER_SYSTEM( InputSystem, "input" );
}
