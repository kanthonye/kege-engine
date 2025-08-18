//
//  input-context-manager.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/18/25.
//

#include "input-context-manager.hpp"

namespace kege{

    bool InputContextManager::addContext( const kege::string& sid, kege::Ref< kege::InputContext > context )
    {
        if ( context )
        {
            _contexts.emplace(std::make_pair( sid.c_str(), context ));
            _active_contexts.push_back( context.ref() );
            return true;
        }
        return false;
    }

    void InputContextManager::process( const std::vector< Input >& inputs )
    {
        _mapped_inputs.clear();
        for (const kege::Input& input : inputs )
        {
            kege::Coord2d d;
            if ( input.type == kege::Input::POINTER )
            {
                d.x = input.coord.x - _mapped_inputs._pointer.x;
                d.y = input.coord.y - _mapped_inputs._pointer.y;
                _mapped_inputs._pointer.x = input.coord.x;
                _mapped_inputs._pointer.y = input.coord.y;

                if ( d.x > 0 )
                {
                    update( POINTER_X_POSITIVE, d.x );
                }
                else if ( d.x < 0 )
                {
                    update( POINTER_X_NEGATIVE, d.x );
                }

                if ( d.y > 0 )
                {
                    update( POINTER_Y_POSITIVE, d.y );
                }
                else if ( d.y < 0 )
                {
                    update( POINTER_Y_NEGATIVE, d.y );
                }
            }
            else if ( input.type == kege::Input::SCROLL )
            {
                if ( input.coord.x > 0 )
                {
                    update( SCROLL_X_POSITIVE, input.coord.x );
                }
                else if ( input.coord.x < 0 )
                {
                    update( SCROLL_X_NEGATIVE, input.coord.x );
                }

                if ( input.coord.y > 0 )
                {
                    update( SCROLL_Y_POSITIVE, input.coord.y );
                }
                else if ( input.coord.y < 0 )
                {
                    update( SCROLL_Y_NEGATIVE, input.coord.y );
                }
            }
            else if ( input.type == Input::KEYBOARD || input.type == Input::MOUSEKEY )
            {
                update( input.key.code, input.key.state != 0 );
            }
        }
    }

    void InputContextManager::update( uint16_t keycode, double value )
    {
        for ( kege::InputContext* context : _active_contexts )
        {
            kege::InputCommand* input = context->get( keycode );
            if ( input != nullptr )
            {
                input->amount = value;
                _mapped_inputs.insert( input );
                break;
            }
        }
    }

    bool InputContextManager::initialize( GraphicsWindow* window )
    {
        return _user_input_receiver.initialize( window );
    }

    void InputContextManager::shutdown()
    {
        _mapped_inputs.clear();
        _contexts.clear();
        _active_contexts.clear();
    }

    std::vector< kege::Input >& InputContextManager::getCurrentInputs()
    {
        return _current_inputs;
    }

    MappedInputs& InputContextManager::getMappedInputs()
    {
        return _mapped_inputs;
    }

    void InputContextManager::updateCurrentInputs()
    {
        _current_inputs.clear();
        _user_input_receiver.getInputs( _current_inputs );
        process( _current_inputs );
    }

    InputContextManager::InputContextManager()
    {}
}
