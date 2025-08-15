//
//  graphics-window.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/27/25.
//

#include "../../graphics/core/graphics-window.hpp"

namespace kege{

    void GraphicsWindow::onKeyboard( int key, int scancode, int action, int mods )
    {
        for ( WindowListener* listener : _listeners )
        {
            listener->onKeyboard( key, scancode, action, mods );
        }
    }

    void GraphicsWindow::onMouseButton( int button, int action, int mods )
    {
        for ( WindowListener* listener : _listeners )
        {
            listener->onMouseButton( button, action, mods );
        }
    }

    void GraphicsWindow::onCursorPosition( double xpos, double ypos )
    {
        for ( WindowListener* listener : _listeners )
        {
            listener->onCursorPosition( xpos,  ypos);
        }
    }

    void GraphicsWindow::onScroll( double xoffset, double yoffset )
    {
        for ( WindowListener* listener : _listeners )
        {
            listener->onScroll( xoffset, yoffset );
        }
    }

    void GraphicsWindow::removeListener( WindowListener* listener )
    {
        auto itr = std::find( _listeners.begin(), _listeners.end(), listener );
        if ( itr != _listeners.end() )
        {
            _listeners.erase( itr );
        }
    }
    
    void GraphicsWindow::addListener( WindowListener* listener )
    {
        auto itr = std::find( _listeners.begin(), _listeners.end(), listener );
        if ( itr != _listeners.end() ) return;
        _listeners.push_back( listener );
    }

}




namespace kege{

    void WindowResized::callback(int width, int height)
    {
        _width = width;
        _height = height;
    }

    int WindowResized::getHeight()
    {
        return _height;
    }
    int WindowResized::getWidth()
    {
        return _width;
    }
    int WindowResized::_height = 0;
    int WindowResized::_width = 0;



    void FramebufferResized::callback(int width, int height)
    {
        _width = width;
        _height = height;
    }
    int FramebufferResized::getHeight()
    {
        return _height;
    }
    int FramebufferResized::getWidth()
    {
        return _width;
    }
    
    int FramebufferResized::_height = 0;
    int FramebufferResized::_width = 0;
}
