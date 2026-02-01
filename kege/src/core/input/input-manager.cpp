//
//  input-manager.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/24/26.
//

#include "input-manager.hpp"
namespace kege{

    void InputManager::operator()( const kege::KeyboardEvent& event )
    {
        _keyboard.onKeyboard(event.code, event.scancode, event.action, event.mods);
    }

    void InputManager::operator()( const kege::ButtonEvent& event )
    {
        _mouse.onMouseButton(event.code, event.action, event.mods);
    }

    void InputManager::operator()( const kege::PointerEvent& event )
    {
        _mouse.onCursorPosition(event.x, event.y);
    }

    void InputManager::operator()( const kege::ScrollEvent& event )
    {
        _mouse.onScroll(event.x, event.y);
    }

    const kege::Keyboard* InputManager::getKeyboard()const
    {
        return &_keyboard;
    }

    const kege::Mouse* InputManager::getMouse()const
    {
        return &_mouse;
    }

    bool InputManager::initialize()
    {
        kege::Communication::add<const kege::KeyboardEvent&, InputManager>(this);
        kege::Communication::add<const kege::ButtonEvent&, InputManager>(this);
        kege::Communication::add<const kege::PointerEvent&, InputManager>(this);
        kege::Communication::add<const kege::ScrollEvent&, InputManager>(this);
        return true;
    }

    void InputManager::shutdown()
    {
        kege::Communication::remove<const kege::KeyboardEvent&, InputManager>(this);
        kege::Communication::remove<const kege::ButtonEvent&, InputManager>(this);
        kege::Communication::remove<const kege::PointerEvent&, InputManager>(this);
        kege::Communication::remove<const kege::ScrollEvent&, InputManager>(this);
    }

    void InputManager::beginInput()
    {
        _window->pollEvents();
        _mouse.begin();
        _keyboard.begin();
    }

    void InputManager::endInput()
    {
        _keyboard.end();
        _mouse.end();
    }

    InputManager::InputManager( kege::AppWindow* window )
    :   _window( window )
    {
    }

}
