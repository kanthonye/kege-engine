//
//  input-manager.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/24/26.
//

#ifndef input_manager_hpp
#define input_manager_hpp

#include "../app/app-layer.hpp"
#include "mouse.hpp"
#include "keyboard.hpp"

namespace kege{

    class InputManager : public kege::RefCounter
    {
    public:

        void operator()( const kege::KeyboardEvent& event );
        void operator()( const kege::ButtonEvent& event );
        void operator()( const kege::PointerEvent& event );
        void operator()( const kege::ScrollEvent& event );

        const kege::Keyboard* getKeyboard()const;
        const kege::Mouse* getMouse()const;

        bool initialize();
        void shutdown();

        void beginInput();
        void endInput();

        InputManager( kege::AppWindow* window );

    private:

        kege::Keyboard _keyboard;
        kege::Mouse _mouse;
        AppWindow* _window;
    };
}

namespace kege::ref{
    typedef kege::Ref< kege::InputManager > InputManager;
}
#endif /* input_manager_hpp */
