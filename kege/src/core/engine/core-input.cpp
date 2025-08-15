//
//  input-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#include "../../core/engine/engine.hpp"
#include "core-input.hpp"

namespace kege{

    bool CoreInput::initialize()
    {
        _module = new kege::UserInputReceiver;
        if( !_module->initialize( _engine->graphics()->getWindow() ) )
        {
            KEGE_LOG_ERROR << "Failed to initialize SystemInput.";
            return false;
        }

        string input_file = _engine->vfs()->fetch( "assets/config/keybinds.json" );
        kege::Ref< kege::InputContext > context = kege::InputContextLoader::load( input_file );
        if ( !context )
        {
            KEGE_LOG_ERROR << "CoreInput::initialize " << input_file <<Log::nl;
            return false;
        }
        
        _context_manager.setMappedInputs( &_mapped_inputs );
        _context_manager.addContext( "keybinds", context );
        return System::initialize();
    }

    std::vector< kege::Input >& CoreInput::getCurrentInputs()
    {
        return _current_inputs;
    }
    kege::InputContextManager& CoreInput::getContextManager()
    {
        return _context_manager;
    }
    kege::MappedInputs& CoreInput::getMappedInputs()
    {
        return _mapped_inputs;
    }

    void CoreInput::shutdown()
    {
        if ( _module )
        {
            _module->shutdown();
            _module.clear();
        }
    }

    void CoreInput::update()
    {
        _current_inputs.clear();
        _module->getInputs( _current_inputs );
        _context_manager.process( _current_inputs );
        Communication::broadcast< const MappedInputs& >( _mapped_inputs );
    }

    CoreInput::CoreInput( kege::Engine* engine )
    :   kege::CoreSystem< kege::UserInputReceiver >( engine, "core-input" )
    {
    }

    //KEGE_REGISTER_SYSTEM( InputSystem, "input" );
}
