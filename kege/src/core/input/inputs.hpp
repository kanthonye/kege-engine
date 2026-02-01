//
//  input.hpp
//  vulkan-renderer
//
//  Created by Kenneth Esdaile on 4/17/24.
//

#ifndef kege_input_hpp
#define kege_input_hpp

#include <stdlib.h>
#include <vector>
#include <unordered_map>
#include "keycodes.hpp"

namespace kege{

    enum struct Modifiers : uint8_t
    {
        Shift    = 1,
        Control  = 2,
        Option   = 4,
        Super    = 8,
    };

    inline constexpr bool operator ==(const Modifiers m1, const Modifiers m2 )
    {
        return (static_cast<int>(m1) & static_cast<int>(m2)) != 0;
    }

    inline constexpr bool operator !=(const Modifiers m1, const Modifiers m2 )
    {
        return (static_cast<int>(m1) & static_cast<int>(m2)) == 0;
    }

    enum struct KeyState : uint8_t
    {
        Release   = 0,
        Pressed,
        PressedRepeat,
    };

    struct Key
    {
        uint16_t code;
        uint16_t scancode;
        KeyState action;
        Modifiers mods;
    };

    struct Coord2d
    {
        double x, y;
    };

    struct Input
    {
        enum Type: char{ KEYBOARD, MOUSEKEY, POINTER, SCROLL, ASCII_CODE, TOTAL_TYPE };

        Input& operator=( const Input& other );
        Input( Type typ, uint16_t code, uint16_t state, int mods );
        Input( Type typ, double x, double y );
        Input( const Input& other );
        Input(){}

        union
        {
            Coord2d coord;
            Key     key;
        };

        Type type;
    };

    typedef std::vector< kege::Input > InputEvents;

}

#endif /* kege_input_hpp */
