//
//  keyboard.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/23/26.
//

#include "keyboard.hpp"
namespace kege{

    void Keyboard::onKeyboard( int key, int scancode, int action, int mods )
    {
        if( key >= 384 ) return;

        _modifiers = (Modifiers)mods;
        _keys[ key ].code = key;
        _keys[ key ].scancode = scancode;
        _keys[ key ].action = (KeyState)action;
        _keys[ key ].mods = (Modifiers)mods;


        if ( _active_key_count >= _active_keys.size() )
        {
            _active_keys.resize( 2 * _active_keys.size() + 1 );
        }
        _active_keys[ _active_key_count ] = &_keys[ key ];
        _active_key_count += 1;
    }

    Key const* Keyboard::getActiveKeys() const
    {
        return *_active_keys.data();
    }

    uint32_t Keyboard::getActiveKeyCount() const
    {
        return _active_key_count;
    }

    bool Keyboard::anyKeyDown()const
    {
        return _active_key_count != 0;
    }

    bool Keyboard::isPressed( int key )
    {
        return _keys[key].action == KeyState::Pressed;
    }

    bool Keyboard::isDown( int key )
    {
        return _keys[key].action == KeyState::Pressed || _keys[key].action == KeyState::PressedRepeat;
    }

    void Keyboard::begin()
    {
    }

    void Keyboard::end()
    {
        _active_key_count = 0;
    }

    Modifiers Keyboard::getModifiers() const
    {
        return _modifiers;
    }

    Keyboard::Keyboard()
    {
        _active_keys.resize(16);
        _active_key_count = 0;
    }
}
