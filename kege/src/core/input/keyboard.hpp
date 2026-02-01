//
//  keyboard.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/23/26.
//

#ifndef keyboard_hpp
#define keyboard_hpp

#include "../input/inputs.hpp"
#include "../input/thread-safe-vector.hpp"
#include "../app/app-window.hpp"

namespace kege{

    struct KeyboardKey
    {
        uint16_t key;
        uint16_t scancode;
        KeyState action;
        Modifiers mods;
    };

    class Keyboard : public WindowListener
    {
    public:

        void onKeyboard( int key, int scancode, int action, int mods );

        Key const* getActiveKeys() const;
        uint32_t getActiveKeyCount() const;
        bool anyKeyDown() const;

        bool isPressed( int key );
        bool isDown( int key );

        Modifiers getModifiers() const noexcept;

        void begin();
        void end();

        Keyboard();
        
    private:

        std::vector< const Key* > _active_keys;
        std::array< Key,384 > _keys;
        uint32_t _active_key_count;
    };

}

#endif /* keyboard_hpp */
