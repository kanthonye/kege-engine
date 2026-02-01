//
//  app-window.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#include "app-window.hpp"

namespace kege{

    void AppWindow::onKeyboard( int key, int scancode, int action, int mods )
    {
        for ( WindowListener* listener : _listeners )
        {
            listener->onKeyboard( key, scancode, action, mods );
        }
    }

    void AppWindow::onMouseButton( int button, int action, int mods )
    {
        for ( WindowListener* listener : _listeners )
        {
            listener->onMouseButton( button, action, mods );
        }
    }

    void AppWindow::onCursorPosition( double xpos, double ypos )
    {
        for ( WindowListener* listener : _listeners )
        {
            listener->onCursorPosition( xpos,  ypos);
        }
    }

    void AppWindow::onScroll( double xoffset, double yoffset )
    {
        for ( WindowListener* listener : _listeners )
        {
            listener->onScroll( xoffset, yoffset );
        }
    }

    void AppWindow::removeListener( WindowListener* listener )
    {
        auto itr = std::find( _listeners.begin(), _listeners.end(), listener );
        if ( itr != _listeners.end() )
        {
            _listeners.erase( itr );
        }
    }
    
    void AppWindow::addListener( WindowListener* listener )
    {
        auto itr = std::find( _listeners.begin(), _listeners.end(), listener );
        if ( itr != _listeners.end() ) return;
        _listeners.push_back( listener );
    }

}
