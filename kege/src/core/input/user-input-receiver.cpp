//
//  input-handler.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 1/25/25.
//

#include "user-input-receiver.hpp"

namespace kege{

    void UserInputReceiver::onKeyboard( int key, int scancode, int action, int mods )
    {
        _inputs.push( Input( Input::KEYBOARD, key, action, mods ) );
    }

    void UserInputReceiver::onMouseButton( int button, int action, int mods )
    {
        _inputs.push( Input( Input::MOUSEKEY, button, action, mods ) );
    }

    void UserInputReceiver::onCursorPosition( double xpos, double ypos )
    {
        _inputs.push( Input( Input::POINTER, xpos, ypos ) );
    }

    void UserInputReceiver::onScroll( double xoffset, double yoffset )
    {
        _inputs.push( Input( Input::SCROLL, xoffset, yoffset ) );
    }

    bool UserInputReceiver::initialize( GraphicsWindow* window )
    {
        window->addListener( this );
        return true;
    }

    void UserInputReceiver::getInputs( std::vector< Input >& inputs )
    {
        _inputs.swapOut( inputs );
        _inputs.reset();
    }

    void UserInputReceiver::shutdown()
    {
    }

    UserInputReceiver::UserInputReceiver()
    {
    }

}
