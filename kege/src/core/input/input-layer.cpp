//
//  input-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#include "engine.hpp"
#include "input-layer.hpp"

namespace kege{

    InputLayer::InputLayer( ref::InputContextManager icm )
    :   kege::AppLayer( "InputLayer" )
    ,   _manager( icm )
    {}

    bool InputLayer::initialize()
    {
//        if ( _manager != nullptr )
//        {
//            kege::Log::error << "InputModule already initialized!" << Log::nl;
//            return false;
//        }

//        _manager = new kege::InputContextManager;
//        if( !_manager->initialize( _window.ref() ) )
//        {
//            kege::Log::error << "( INITIALIZATION_FAILED ) -> InputContextManager" << Log::nl;
//            return false;
//        }

        string input_file = kege::vfs( "assets/config/keybinds.json" );
        kege::Ref< kege::InputContext > context = kege::InputContextLoader::load( input_file );
        if ( !context )
        {
            kege::Log::error << "( LOADING_FAILED ) -> assets/config/keybinds.json" << Log::nl;
            return false;
        }

        _manager->addContext( "keybinds", context );
        return true;
    }

    void InputLayer::update()
    {
        _manager->updateCurrentInputs();
        /*
         I have two options. i can forward both messages or i can for the list of inputs. the editor layer, and
         the entity input system will get it. both can process the input and do what it wants with it.
         */
        //Communication::broadcast< const std::vector< kege::Input >& >( _manager->getCurrentInputs() );
        //Communication::broadcast< const MappedInputs& >( _manager->getMappedInputs() );
    }

    void InputLayer::shutdown()
    {
        if ( _manager )
        {
            _manager->shutdown();
        }
        _manager.clear();
    }

//    void InputModule::add()
//    {
//        _engine->addModule( this );
//        kege::Log::info << "InputModule module added to engine" << Log::nl;
//    }
//
//    InputModule::InputModule( kege::Engine* engine )
//    :   Module( engine, "InputModule" )
//    {
//    }

}
